#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {
public:
    typedef Pool<Timer> TimerPool;

private:
    TimerPool* timerPool;
    TimerPool::_t timerHandle;
    ThreadControl* slaveThread;
    ThreadControl::_t slaveSequence;
    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        slaveThread->commandStop(slaveSequence);

        timerPool->release(std::move(timerHandle));
    }

public:
    static void run(TimerPool& timerPool, Clock::_t duration, ThreadControl& slaveThread, ThreadControl::_t slaveSequence) {
        //zero duration means no timer
        if (!slaveSequence || duration == Clock::_t::zero()) {
            return;
        }

        TimerPool::_t timerHandle = timerPool.acquire();
        Timer& timer = timerPool.fetch(timerHandle);
        assert (timer.isReady());

        timer.timerPool = &timerPool;
        timer.timerHandle = timerHandle;
        timer.slaveThread = &slaveThread;
        timer.slaveSequence = slaveSequence;
        timer.duration = duration;

        timer.commandRun();
    }
};

#endif
