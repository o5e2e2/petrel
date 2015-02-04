#ifndef TIMER_THREAD_HPP
#define TIMER_THREAD_HPP

#include "ThreadControl.hpp"
#include "TimerPool.hpp"
#include "Clock.hpp"

class TimerThread : public ThreadControl {
    TimerPool::_t thisTimer;
    ThreadControl* slaveThread;
    Clock::_t duration;

    void thread_body() override;

public:
    void start(TimerPool::_t, ThreadControl&, Clock::_t);
};

#endif
