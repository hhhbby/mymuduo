#include <functional>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include "TcpConnection.h"
#include "TimeStamp.h"      // 这里是必需的，因为handleRead中需要，最重要的是TcpConnection::TcpConnection中bind时需要，否则会出错：无法转换
#include "Logger.h"
#include "EventLoop.h"


sockaddr_in getLocalAddr(int fd)
{
    sockaddr_in addr{};
    socklen_t addrlen = sizeof(addr);
    if (getsockname(fd, reinterpret_cast<sockaddr*>(&addr), &addrlen) == -1)
    {
        LOG_ERROR("getLocalAddr failed, errno:%d", errno);
    }
    return addr;
}

TcpConnection::TcpConnection(EventLoop *loop, int fd, InetAddr peerAddr):
    loop_(loop),
    fd_(fd),
    channel_(loop, fd),
    state_(kConnecting),
    localAddr_(getLocalAddr(fd)),
    peerAddr_(peerAddr)
{
    channel_.setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_.setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_.setErrorCallback(std::bind(&TcpConnection::handleError, this));
    channel_.setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    // channel_.enableReading();
    // channel_.enableWriting();
}

TcpConnection::~TcpConnection()
{
    ::close(fd_);
}

void TcpConnection::send(const char *data, size_t len)
{
    if (loop_->isInLoopThread())
        sendInLoop(data, len);
    else {
        auto self = shared_from_this();
        loop_->runInLoop(std::bind(
            [self](const std::string& msg) {
                self->sendInLoop(msg.data(), msg.size());
            }, 
            std::string(data, len)));
    }
}

void TcpConnection::sendInLoop(const char *data, size_t len)
{
    loop_->assertInLoopThread();
    if (state_ != kConnected)
        return;
    // try write directly
    outputBuffer_.append(data, len);
    if (!channel_.isWriting() && outputBuffer_.readableBytes() == len)
    {
        int savedErrno = 0;
        int n = outputBuffer_.writeFd(fd_, &savedErrno);
        if (n >= 0){
            if (outputBuffer_.readableBytes() == 0)
            {}
        } else {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK)
                LOG_ERROR("TcpConnection::send failed");
        }
    }
    if (outputBuffer_.readableBytes() > 0)
    {
        if (!channel_.isWriting())
            channel_.enableWriting();
    }
}

void TcpConnection::connectEstablish()
{
    assert(state_ == kConnecting);
    state_ = kConnected;

    channel_.tie(shared_from_this());

    // enable reading in connecting
    channel_.enableReading();
    
    connectionCallback_(shared_from_this());
}

// called in messageCallback, in subLoop thread
void TcpConnection::shutdown()
{
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
}

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if (state_ != kConnected)
        return;
    state_ = kDisConnecting;
    if (!channel_.isWriting())
    {
        ::shutdown(fd_, SHUT_WR);
    }
}

void TcpConnection::connectDestroyed()
{
    if (state_ == kConnected)
    {
        state_ = kDisConnected;
        channel_.disableAll();
    }
    channel_.remove();
}

void TcpConnection::handleRead(TimeStamp receiveTime)
{
    int savedErrno = 0;
    int n = inputBuffer_.readFd(fd_, &savedErrno);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        if (savedErrno != EAGAIN || savedErrno != EWOULDBLOCK)
        {
            LOG_FATAL("TcpConnection::handleRead");
            handleError();
        }
    }
}

void TcpConnection::handleWrite()
{
    // POLLIN(close) ＆ POLLOUT, then read return 0
    if (!channel_.isWriting())
    {
        LOG_INFO("connection %s is down, no more writing", peerAddr_.toIpPort().c_str());
        return;
    }
    // from outputBuffer_ to send
    int savedErrno = 0;
    int n = outputBuffer_.writeFd(fd_, &savedErrno);
    if (n > 0)
    {
        if (outputBuffer_.readableBytes() == 0)
        {
            channel_.disableWriting();
            if (state_ == kDisConnecting)
                shutdown();
        }
    } else {
        LOG_ERROR("TcpConnection::handleWrite failed, errno: %d", savedErrno);
    }
}

void TcpConnection::handleError()
{

}

void TcpConnection::handleClose()
{
    state_ = kDisConnected;
    channel_.disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    // callback user specified
    connectionCallback_(guardThis);
    // callback TcpServer specified
    closeCallback_(guardThis);

    /*
    * 下面这种不能保证连接活到结束
    * 假设第一个回调释放了其他所有强引用，又没有外层持有者，那么第一个分号处就可能析构连接
    * connectionCallback_(shared_from_this());
    * closeCallback_(shared_from_this());
    */
}
