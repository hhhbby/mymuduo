#pragma once

#include <vector>
#include <functional>
#include <mutex>

#include "Channel.h"

class Poller;
class TimeStamp;
class Channel;

class EventLoop {
public:
    using ChannelList = std::vector<Channel*>;
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();
    void loop();
    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread();
    void assertInLoopThread();
    void runInLoop(Functor f);
    void queueInLoop(Functor f);
private:
    void doPendingFunctors();
    void readWakeupFd();

    Poller* poller_;
    bool looping_;
    bool quit_;
    ChannelList activateChannels_;

    int wakeupFd_;
    Channel wakeupChannel_;

    int tid_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};