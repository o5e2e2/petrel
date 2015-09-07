#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {
    typedef Pool<Timer> TimerPool;
    static TimerPool timerPool;

    TimerPool::_t timerHandle;

    ThreadControl* slaveThread;
    ThreadControl::_t slaveSequence;
    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);

        slaveThread->commandStop(slaveSequence);
        timerPool.release(std::move(timerHandle));
    }

    void start(Clock::_t d, ThreadControl& thread, ThreadControl::_t seq) {
        slaveThread = &thread;
        slaveSequence = seq;
        duration = d;

        assert (isReady());
        commandRun();
    }

    static Timer& acquire() {
        TimerPool::_t timerHandle = timerPool.acquire();
        Timer& timer = TimerPool::fetch(timerHandle);
        timer.timerHandle = timerHandle;
        return timer;
    }

public:
    static ThreadControl::_t start(ThreadControl& slaveThread, Clock::_t duration) {
        ThreadControl::_t slaveSequence = slaveThread.commandRun();

        //zero duration means no timer
        if (slaveSequence && duration != Clock::_t::zero()) {
            Timer::acquire().start(duration, slaveThread, slaveSequence);
        }

        return slaveSequence;
    }

};

#endif
