#pragma once

#include <vector>

#include "EventLoopThread.h"

class EventLoopThreadPool {
public:
    EventLoopThreadPool();
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads);
    void start();
    EventLoop* getNextLoop();
private:
    EventLoop* baseLoop_;    // main loop
    std::vector<EventLoopThread> threads;
    std::vector<EventLoop*> loops_;
    int numThreads_;
    int next_;
};