#include "Channel.h"
#include "TimeStamp.h"
#include "EventLoop.h"

#include <poll.h>

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    state_(-1),
    tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent(TimeStamp receiveTime)
{
    handleEventWithGuard(receiveTime);
}

void Channel::handleEventWithGuard(TimeStamp receiveTime)
{
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (closeCallback_)
            closeCallback_();
    }
    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
            readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }    
}

void Channel::tie(std::shared_ptr<void> obj)
{
    tie_ = obj;
    tied_ = true;
}

int Channel::fd()
{
    return fd_;
}

int Channel::events()
{
    return events_;
}

void Channel::setRevents(int revents)
{
    revents_ = revents;
}

int Channel::state()
{
    return state_;
}

void Channel::setState(int state)
{
    state_ = state;
}

// delete from poller
void Channel::remove()
{
    loop_->removeChannel(this);
}

// add to poller or update events_ to poller
void Channel::update()
{
    loop_->updateChannel(this);
}