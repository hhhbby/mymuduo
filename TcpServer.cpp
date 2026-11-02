#include "TcpServer.h"
#include "InetAddr.h"

TcpServer::TcpServer(EventLoop* loop, InetAddr addr):
    loop_(loop),
    acceptor_(loop, addr),
    pool_()
{

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
    pool_.start();
}

void TcpServer::newConnection(int fd, InetAddr addr)
{
    
}
