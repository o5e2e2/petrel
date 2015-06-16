#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {
    typedef Pool<Timer> TimerPool;
    static TimerPool timerPool;

    TimerPool::_t timerSlot;

    ThreadControl* slaveThread;
    ThreadControl::_t slaveSequence;
    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        slaveThread->commandStop(slaveSequence);
        timerPool.release(std::move(timerSlot));
    }

    void set(Clock::_t d, ThreadControl& thread, ThreadControl::_t seq) {
        assert (isReady());

        slaveThread = &thread;
        slaveSequence = seq;
        duration = d;
    }

public:
    static ThreadControl::_t start(ThreadControl& slaveThread, Clock::_t duration) {
        ThreadControl::_t slaveSequence = slaveThread.commandRun();

        //zero duration means no timer
        if (duration != Clock::_t::zero()) {
            TimerPool::_t timerSlot = timerPool.acquire();
            Timer& timer = TimerPool::fetch(timerSlot);
            timer.timerSlot = timerSlot;

            timer.set(duration, slaveThread, slaveSequence);
            timer.commandRun();
        }

        return slaveSequence;
    }

};

#endif
