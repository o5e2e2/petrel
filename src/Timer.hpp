#ifndef TIMER_HPP
#define TIMER_HPP

#include "TimerPool.hpp"
#include "Clock.hpp"

class ThreadControl;

class Timer {
    TimerPool::_t thisTimer;

    Timer (const Timer&) = delete;
    Timer& operator = (const Timer&) = delete;

public:
    Timer () {}
   ~Timer () { cancel(); }

    void start(ThreadControl& slaveThread, Clock::_t duration);
    void cancel();
};

#endif
