#include "Acceptor.h"
#include "InetAddr.h"
#include "Logger.h"
#include "TimeStamp.h"

#include <unistd.h>

void setReuseAddr(int fd, bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // check
}

int createNonblockOrDie()
{
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0)
        LOG_FATAL("createNonblockOrDie failed");
    return fd;
}

void bindOrDie(int fd, InetAddr &addr)
{
    if (::bind(fd, reinterpret_cast<sockaddr *>(addr.addr()), sizeof(sockaddr)) < 0)
        LOG_FATAL("bindOrDie failed");
}

Acceptor::Acceptor(EventLoop *loop, InetAddr addr) : 
    fd_(createNonblockOrDie()),
    channel_(loop, fd_)
{
    setReuseAddr(fd_, true);
    bindOrDie(fd_, addr);
    channel_.setReadCallback(std::bind(&Acceptor::handleRead, this, std::placeholders::_1));
}

Acceptor::~Acceptor()
{
    close(fd_);
}

void Acceptor::listen()
{
    if (::listen(fd_, SOMAXCONN) == -1)
        LOG_FATAL("listen failed");
    channel_.enableReading();
}

void Acceptor::setNewConnectionCallback(newConnectionCallback cb)
{
    newConnectionCallback_ = cb;
}

void Acceptor::handleRead(TimeStamp t)
{
    sockaddr_in peerAddr{};
    socklen_t len = sizeof(peerAddr);
    int clientFd = accept4(fd_, reinterpret_cast<sockaddr*>(&peerAddr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (clientFd == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return;
        LOG_FATAL("new connection coming, but accept failed");
    }
    InetAddr addr(peerAddr);
    LOG_INFO("new connection from %s at %s", addr.toIpPort().c_str(), t.toString().c_str());
    if (newConnectionCallback_)
    {
        newConnectionCallback_(clientFd, addr);
    }
}