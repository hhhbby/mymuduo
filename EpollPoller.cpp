#include <unistd.h>
#include <strings.h>
#include <string.h>

#include "EpollPoller.h"
#include "Logger.h"
#include "TimeStamp.h"
#include "Channel.h"

EpollPoller::EpollPoller(EventLoop* loop)
    :Poller(loop),
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(16)
{
    if (epollfd_ == -1)
        LOG_FATAL("epoll_create1 failed");
}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

void EpollPoller::updateChannel(Channel* channel)
{
    int state = channel->state();
    int fd = channel->fd();
    if (state == kNew || state == kDeleted)
    {
        if (state == kNew)
        {
            channels_[fd] = channel;
        }
        channel->setState(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        if (channel->isNoneEvent())
        {
            channel->setState(kDeleted);
            update(EPOLL_CTL_DEL, channel);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel)
{
    channels_.erase(channel->fd());
    if (channel->state() == kAdded)
        update(EPOLL_CTL_DEL, channel);
    channel->setState(kNew);
}

TimeStamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int ret = epoll_wait(epollfd_, events_.data(), events_.size(), timeoutMs);
    TimeStamp t(TimeStamp::now());
    if (ret == -1)
    {
        if (errno != EINTR)
            LOG_ERROR("epoll_wait failed : %s", strerror(errno));
    } else if (ret > 0) {
        fillActiveChannels(ret, activeChannels);
        if (static_cast<size_t>(ret) == events_.size())
            events_.resize(2 * events_.size());
    } else {
        ;   // timeout, nothing happen
    }
    return t;
}

void EpollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    if (epoll_ctl(epollfd_, operation,channel->fd(), &event) == -1)
    {
        if (errno != EINTR)
            LOG_ERROR("epoll_ctl failed : %s", strerror(errno));
    }
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels)
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}
