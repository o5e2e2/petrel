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

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        thread->stop(sequence);
        pool->release(std::move(handle));
    }
};

class Timer : private TimerPool {
public:
    void start(Duration duration, ThreadControl& thread, ThreadControl::Sequence sequence) {
        //zero duration means no timer
        if (!sequence || duration == Duration::zero()) {
            return;
        }

        auto handle = acquire();
        auto& timer = fetch(handle);
        assert (timer.isReady());

        timer.pool = this;
        timer.handle = handle;
        timer.thread = &thread;
        timer.sequence = sequence;
        timer.duration = duration;

        timer.start();
    }
};

#endif
