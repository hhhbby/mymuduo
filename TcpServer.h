#pragma once

#include "Acceptor.h"
#include "EventLoopThreadPool.h"

class TimeStamp;
class InetAddr;
class EventLoop;

class TcpServer {
public:
    TcpServer(EventLoop* loop, InetAddr addr);
    ~TcpServer();
    void setThreadNum(int numThreads);
    void start();
private:
    void newConnection(int fd, InetAddr addr);
    EventLoop* loop_;
    Acceptor acceptor_;
    EventLoopThreadPool pool_;
};