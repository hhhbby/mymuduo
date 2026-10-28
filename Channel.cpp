#include "Channel.h"
#include "TimeStamp.h"

#include <poll.h>

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent(TimeStamp receiveTime)
{

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

void Channel::remote()
{
}

void Channel::update()
{

}