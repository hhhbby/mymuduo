#pragma once

#include <vector>
#include <unordered_map>

#include "TimeStamp.h"
class EventLoop;
class Channel;

class Poller {
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop):loop_(loop) {}
    virtual ~Poller() = default;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual TimeStamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    std::unordered_map<int, Channel*> channels_;
private:
    EventLoop* loop_;
};