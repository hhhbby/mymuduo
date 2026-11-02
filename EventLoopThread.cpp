#include <functional>

#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Logger.h"

EventLoopThread::EventLoopThread()
    : thread_(std::bind(&EventLoopThread::threadFunc, this)),
    loop_(nullptr),
    sem_()
{
    if (sem_init(&sem_, 0, 0) == -1)
    {
        LOG_FATAL("%s:%s sem_init failed", __FILE__, __LINE__);
    }    
}

EventLoopThread::~EventLoopThread()
{
    sem_destroy(&sem_);
}

EventLoop *EventLoopThread::startLoop()
{
    thread_.start();
    sem_wait(&sem_);
    EventLoop* loop = loop_;
    return loop;
}

void EventLoopThread::threadFunc()
{
    // run in subthread with parameter this
    EventLoop loop;
    this->loop_ = &loop;
    sem_post(&sem_);

    loop.loop();
}
