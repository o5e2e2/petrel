#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : public ThreadControl {
    typedef Pool<Timer> TimerPool;
    static TimerPool timerPool;

    TimerPool::element_type thisTimer;
    ThreadControl* slaveThread;
    sequence_t slaveSequence;

    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);

        //signal timeout only if the timer is still active
        slaveThread->commandStop(slaveSequence);

        //inform the pool about idle timer
        timerPool.release(std::move(thisTimer));
    }

    void start(TimerPool::element_type t, ThreadControl& s, sequence_t seq, Clock::_t d) {
        thisTimer = t;
        slaveThread = &s;
        slaveSequence = seq;
        duration = d;

        this->commandRun();
    }

public:
    static void start(ThreadControl& slaveThread, ThreadControl::sequence_t seq, Clock::_t duration) {
        if (duration == Clock::_t::zero()) { return; } //zero duration means no timer

        auto thisTimer = timerPool.acquire();
        timerPool[thisTimer].start(thisTimer, slaveThread, seq, duration);
    }

};

#endif
