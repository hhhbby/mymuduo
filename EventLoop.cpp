#include <sys/eventfd.h>
#include <functional>
#include <unistd.h>

#include "Poller.h"
#include "EventLoop.h"
#include "Logger.h"
#include "CurrentThread.h"


const int kPollTimeMs = 10000; // 10000毫秒 = 10秒钟

int createEventFd()
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd == -1)
        LOG_FATAL("create eventfd failed");
    return fd;
}

EventLoop::EventLoop():
    poller_(Poller::newDefaultPoller(this)),
    looping_(false),
    quit_(false),
    wakeupFd_(createEventFd()),
    wakeupChannel_(this, wakeupFd_),
    tid_(CurrentThread::tid())
{
    wakeupChannel_.setReadCallback(std::bind(&EventLoop::readWakeupFd, this));
    // always reading
    wakeupChannel_.enableReading();
}

EventLoop::~EventLoop()
{
    close(wakeupFd_);
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        activateChannels_.clear();
        TimeStamp pollReturnTime = poller_->poll(kPollTimeMs, &activateChannels_);
        for (Channel* channel : activateChannels_)
        {
            channel->handleEvent(pollReturnTime);
        }
        doPendingFunctors();
        //
    }
}

void EventLoop::wakeup()
{
    uint64_t counter = 1;
    int n = read(wakeupFd_, &counter, sizeof(uint64_t));
    if (n != sizeof(uint64_t))
    {
        LOG_FATAL("write wakeupFd_ failed");
    }
}

void EventLoop::runInLoop(Functor f)
{
    if (tid_ == CurrentThread::tid())
        f();
    else
        queueInLoop(f);
}

void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::queueInLoop(Functor f)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pendingFunctors_.push_back(std::move(f));

    // if ()
        wakeup();
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> fs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fs.swap(pendingFunctors_);
    }
    for (auto& f : fs)
        f();
}

void EventLoop::readWakeupFd()
{
    uint64_t counter = 0;
    int n = read(wakeupFd_, &counter, sizeof(uint64_t));
    if (n != sizeof(uint64_t))
    {
        LOG_FATAL("read wakeupFd_ failed");
    }
}
