#ifndef TIMER_HPP
#define TIMER_HPP

#include "Duration.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class TimerThread;
typedef Pool<TimerThread> TimerPool;

class TimerThread : private ThreadControl {
    friend class Timer;

    TimerPool* pool;
    TimerPool::Handle handle;
    ThreadControl* thread;
    TaskId taskId;
    Duration duration;

    void run() override;
};

class Timer : private TimerPool {
public:
    void start(Duration, ThreadControl&, ThreadControl::TaskId);
};

#endif
