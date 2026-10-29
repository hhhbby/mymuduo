#pragma once

#include <sys/epoll.h>

#include "Poller.h"

class EpollPoller: public Poller
{
public:
    EpollPoller(EventLoop* loop);
    ~EpollPoller() override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
    TimeStamp poll(int timeoutMs, ChannelList* activeChannels) override;
private:
    void update(int operation, Channel* channel);
    void fillActiveChannels(int numEvents, ChannelList* activeChannels);
    static const int kNew = -1;
    static const int kAdded = 1;
    static const int kDeleted = 2;

    int epollfd_;
    std::vector<epoll_event> events_;
    static const int kInitEventListSize = 16;
};