#ifndef TIMER_HPP
#define TIMER_HPP

#include "typedefs.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {
public:
    typedef Pool<Timer> TimerPool;

private:
    TimerPool* timerPool;
    TimerPool::_t timerHandle;
    ThreadControl* thread;
    ThreadControl::Sequence sequence;
    Duration duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        thread->commandStop(sequence);

        timerPool->release(std::move(timerHandle));
    }

public:
    static void run(TimerPool& timerPool, Duration duration, ThreadControl& thread, ThreadControl::Sequence sequence) {
        //zero duration means no timer
        if (!sequence || duration == Duration::zero()) {
            return;
        }

        TimerPool::_t timerHandle = timerPool.acquire();
        Timer& timer = timerPool.fetch(timerHandle);
        assert (timer.isReady());

        timer.timerPool = &timerPool;
        timer.timerHandle = timerHandle;
        timer.thread = &thread;
        timer.sequence = sequence;
        timer.duration = duration;

        timer.commandRun();
    }
};

#endif
