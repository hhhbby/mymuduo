#pragma once
#include <memory>   // 

#include "InetAddr.h"
#include "Buffer.h"
#include "Channel.h"
#include "callbacks.h"
#include "noncopyable.h"


class EventLoop;


class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, int fd, InetAddr peerAddr);
    ~TcpConnection();
    void send(const char* data, size_t len);

    void setMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
    void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }

    void connectEstablish();
    bool connected() const { return state_ == kConnected; };
    void shutdown();

    EventLoop* getLoop() { return loop_; }

    // called by ~TcpServer and TcpConnection::handleClose, that is why inner has "if"
    void connectDestroyed();
    
private:
    void handleRead(TimeStamp receiveTime);
    void handleWrite();
    void handleError();
    void handleClose();

    enum STATE_E { kConnecting, kConnected, kDisConnecting, kDisConnected };

    EventLoop* loop_;
    int fd_;
    Channel channel_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    STATE_E state_;    
    InetAddr localAddr_;
    InetAddr peerAddr_;

    // callbacks from TcpServer
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};