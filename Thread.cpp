#include "Thread.h"
#include "CurrentThread.h"
#include "Logger.h"

#include <semaphore.h>
#include <assert.h>

int Thread::numCreated_ = 0;

Thread::Thread(ThreadFunc func, const std::string& name):
    name_(name),
    func_(std::move(func)),
    tid_(),
    started_(false),
    thread_(nullptr)
{
    setDefaultName();
}
Thread::~Thread()
{
    if (thread_ && !thread_->joinable())
    {
        thread_->detach();
    }
}
void Thread::start()
{
    started_ = true;
    sem_t sem;
    if (sem_init(&sem, 0, 0) == -1)
    {
        LOG_FATAL("%s:%d sem_init failed", __FILE__, __LINE__);
    }
    thread_ = std::make_shared<std::thread>([this, &sem](){
        this->tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_();
    });
    // ensure tid_ is available
    sem_wait(&sem);
    sem_destroy(&sem);
    assert(tid_ > 0);
}
void Thread::join()
{
    if (thread_->joinable())
        thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        name_ = "thread" + std::to_string(num);
    }
}