#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : public ThreadControl {
    typedef Pool<Timer> TimerPool;
    static TimerPool timerPool;

    TimerPool::_t thisTimer;
    ThreadControl* slaveThread;
    sequence_t slaveSequence;

    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        slaveThread->commandStop(slaveSequence);
        timerPool.release(std::move(thisTimer));
    }

    void start(TimerPool::_t timer, Clock::_t d, ThreadControl& thread, sequence_t seq) {
        assert(&TimerPool::fetch(timer) == this);

        thisTimer = timer;
        duration = d;

        slaveThread = &thread;
        slaveSequence = seq;

        this->commandRun();
    }

public:
    static void start(Clock::_t duration, ThreadControl& slaveThread, ThreadControl::sequence_t seq) {
        if (duration == Clock::_t::zero()) { return; } //zero duration means no timer

        TimerPool::_t timer = timerPool.acquire();
        TimerPool::fetch(timer).start(timer, duration, slaveThread, seq);
    }

};

#endif
