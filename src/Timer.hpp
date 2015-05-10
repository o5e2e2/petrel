#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {
    typedef Pool<Timer> TimerPool;
    static TimerPool timerPool;

    TimerPool::_t selfIterator;
    ThreadControl* slaveThread;
    sequence_t slaveSequence;

    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        slaveThread->commandStop(slaveSequence);
        timerPool.release(std::move(selfIterator));
    }

    void run(TimerPool::_t self, Clock::_t d, ThreadControl& thread, sequence_t seq) {
        assert(&TimerPool::fetch(self) == this);

        selfIterator = self;
        duration = d;

        slaveThread = &thread;
        slaveSequence = seq;

        this->commandRun();
    }

public:
    static void start(Clock::_t duration, ThreadControl& slaveThread, ThreadControl::sequence_t seq) {
        if (duration == Clock::_t::zero()) { return; } //zero duration means no timer

        TimerPool::_t link = timerPool.acquire();
        TimerPool::fetch(link).run(link, duration, slaveThread, seq);
    }

};

#endif
