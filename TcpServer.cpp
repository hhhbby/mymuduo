#include <functional>
#include <assert.h>

#include "TcpServer.h"
#include "InetAddr.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Logger.h"

TcpServer::TcpServer(EventLoop* loop, InetAddr addr):
    loop_(loop),
    acceptor_(loop, addr),
    pool_(loop)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    LOG_INFO("TcpServer ctor");
}

TcpServer::~TcpServer()
{
    while (!connections_.empty())
    {
        auto it = connections_.begin();
        TcpConnectionPtr conn = *it;
        connections_.erase(it);
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    pool_.setThreadNum(numThreads);
}

void TcpServer::start()
{
    acceptor_.listen();
    pool_.start();
}

// run in  io loop
void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    loop_->assertInLoopThread();
    assert(connections_.erase(conn) == 1);
    EventLoop* ioLoop = conn->getLoop();

    // if no sub loop, we destroy after Channel::handleEvent()， in doPendingFunctor()
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));;
}

// run in main loop
void TcpServer::newConnection(int fd, InetAddr addr)
{
    loop_->assertInLoopThread();

    EventLoop* ioLoop = pool_.getNextLoop();
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, fd, addr);

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    // make sure new conn not destory
    connections_.insert(conn);
    // run in ioLoop
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablish, conn));
}
