#include "CurrentThread.h"

#include <unistd.h>
#include <syscall.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace CurrentThread {
    thread_local int t_cachedTid = 0;

    void cacheTid()
    {
        t_cachedTid = syscall(SYS_gettid);
    }

    int tid()
    {
        if (unlikely(t_cachedTid == 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}
