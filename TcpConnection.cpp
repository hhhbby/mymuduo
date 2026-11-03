#include <functional>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>

#include "TcpConnection.h"
#include "TimeStamp.h"      // 这里是必需的，因为handleRead中需要，最重要的是TcpConnection::TcpConnection中bind时需要，否则会出错：无法转换
#include "Logger.h"


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
}

void TcpConnection::send(const char *data, size_t len)
{
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
                LOG_FATAL("TcpConnection::send failed");
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
    state_ = kDisConnecting;
    if (!channel_.isWriting())
    {
        ::shutdown(fd_, SHUT_WR);
    }
}

void TcpConnection::connectDestroyed()
{
    // bug
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
        LOG_FATAL("TcpConnection::handleWrite failed");
    }
}

void TcpConnection::handleError()
{

}

void TcpConnection::handleClose()
{
    state_ = kDisConnected;
    channel_.disableAll();

    // callback user specified
    connectionCallback_(shared_from_this());
    // callback TcpServer specified
    // bug
    closeCallback_(shared_from_this());
}
