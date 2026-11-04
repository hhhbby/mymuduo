#include "EventLoopThreadPool.h"


EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop)
    :baseLoop_(loop),
    threads(),
    numThreads_(0),
    next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::setThreadNum(int numThreads)
{
    numThreads_ = numThreads;
}

void EventLoopThreadPool::start()
{
    for (int i = 0; i < numThreads_; ++i)
    {
        EventLoopThread t;
        loops_.push_back(t.startLoop());
        threads.push_back(t);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
    if (numThreads_ == 0)
        return baseLoop_;
    return loops_[next_++ % numThreads_];
}
