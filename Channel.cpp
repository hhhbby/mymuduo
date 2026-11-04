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
    // 从未绑定对象的 Channel 可以直接分发事件；已经绑定且 lock() 成功，就由 guard 持有对象直到分发结束；
    // 已经绑定但 lock() 失败，就必须跳过分发。
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (!guard)
            return;
    }
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