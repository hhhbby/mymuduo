#include <functional>

#include "TcpServer.h"
#include "InetAddr.h"
#include "TcpConnection.h"
#include "EventLoop.h"

TcpServer::TcpServer(EventLoop* loop, InetAddr addr):
    loop_(loop),
    acceptor_(loop, addr),
    pool_()
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{

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
    for (auto it = conns_.begin(); it != conns_.end(); ++it)
    {
        if (*it == conn)
        {
            conns_.erase(it);
            break;
        }
    }
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));;
}

// run in main loop
void TcpServer::newConnection(int fd, InetAddr addr)
{
    EventLoop* ioLoop = pool_.getNextLoop();
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, fd, addr);\

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    // make sure new conn not destory
    conns_.push_back(conn);
    // run in ioLoop
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablish, conn));
}
