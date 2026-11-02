#pragma once

#include <semaphore.h>

#include "Thread.h"
class EventLoop;

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();
    Thread thread_;
    EventLoop* loop_;
    sem_t sem_;
};