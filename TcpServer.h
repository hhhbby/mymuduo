#pragma once
#include <unordered_set>

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
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
private:
    void newConnection(int fd, InetAddr addr);
    EventLoop* loop_;
    Acceptor acceptor_;
    EventLoopThreadPool pool_;

    // without mutex, so modify it only in main loop
    std::unordered_set<TcpConnectionPtr> connections_;

    // callbacks from user to TcpConnection
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};