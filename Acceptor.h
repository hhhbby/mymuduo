#pragma once

#include "Channel.h"

#include <functional>

class TimeStamp;
class InetAddr;

class Acceptor {
public:
    using newConnectionCallback = std::function<void(int, InetAddr)>;

    Acceptor(EventLoop* loop, InetAddr addr);
    ~Acceptor();
    void listen();
    void setNewConnectionCallback(newConnectionCallback cb);
private:
    void handleRead(TimeStamp t);
    int fd_;
    Channel channel_;
    newConnectionCallback newConnectionCallback_;
};