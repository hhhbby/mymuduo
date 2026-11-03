#pragma once
#include <vector>

#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "callbacks.h"

class TimeStamp;
class InetAddr;
class EventLoop;

class TcpServer {
public:
    TcpServer(EventLoop* loop, InetAddr addr);
    ~TcpServer();
    void setThreadNum(int numThreads);
    void start();

    void setMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
    void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }
    
    void removeConnection(const TcpConnectionPtr& conn);
private:
    void newConnection(int fd, InetAddr addr);
    EventLoop* loop_;
    Acceptor acceptor_;
    EventLoopThreadPool pool_;

    std::vector<TcpConnectionPtr> conns_;

    // callbacks from user to TcpConnection
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};