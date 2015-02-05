#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer {
    class TimerThread;
    typedef Pool<TimerThread> TimerPool;
    static TimerPool timerPool;

    class TimerThread : public ThreadControl {
        TimerPool::element_type thisTimer;
        ThreadControl* slaveThread;
        Clock::_t duration;

        void thread_body() override {
            std::this_thread::sleep_for(duration);

            if (!this->isStopped()) {
                //signal timeout only if the timer is still active
                slaveThread->commandStop();

                //wait for release
                this->waitStop();
            }

            //inform the pool about idle timer
            timerPool.release(std::move(thisTimer));
        }

    public:
        void start(TimerPool::element_type t, ThreadControl& s, Clock::_t d) {
            thisTimer = t;
            slaveThread = &s;
            duration = d;

            this->commandRun();
        }
    };

    TimerPool::element_type thisTimer;

    Timer (const Timer&) = delete;
    Timer& operator = (const Timer&) = delete;

public:
    Timer () {}
   ~Timer () { cancel(); }

    void start(ThreadControl& slaveThread, Clock::_t duration) {
        cancel();

        //zero duration means no timer
        if (duration != Clock::_t::zero()) {
            thisTimer = timerPool.acquire();
            timerPool[thisTimer].start(thisTimer, slaveThread, duration);
        }
    }

    void cancel() {
        if (!timerPool.empty(thisTimer)) {
            //inform the timer that it is free now
            timerPool[thisTimer].commandStop();

            //now we can forget about this timer
            timerPool.clear(thisTimer);
        }
    }

};

#endif
