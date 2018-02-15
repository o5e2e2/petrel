#ifndef TIMER_HPP
#define TIMER_HPP

#include "typedefs.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class TimerThread;
typedef Pool<TimerThread> TimerPool;

class TimerThread : private ThreadControl {
    friend class Timer;

    TimerPool* pool;
    TimerPool::Handle handle;
    ThreadControl* thread;
    ThreadControl::Sequence sequence;
    Duration duration;

    void thread_body() override;
};

class Timer : private TimerPool {
public:
    void start(Duration, ThreadControl&, ThreadControl::Sequence);
};

#endif
