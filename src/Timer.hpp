#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include "Clock.hpp"
#include "Pool.hpp"
#include "SearchThread.hpp"
#include "ThreadControl.hpp"

class Timer {
    class TimerThread;
    typedef Pool<TimerThread> TimerPool;
    static TimerPool timerPool;

    class TimerThread : public ThreadControl<TimerThread> {
        TimerPool::element_type thisTimer;
        SearchThread* searchThread;
        duration_t duration;

    public:
        void thread_body() {
            std::this_thread::sleep_for(duration);

            if (!this->isStopped()) {
                //signal timeout only if the timer is still active
                searchThread->commandStop();

                //wait for release
                this->waitStop();
            }

            //inform the pool about idle timer
            timerPool.release(std::move(thisTimer));
        }

        void start(TimerPool::element_type t, SearchThread& s, duration_t d) {
            thisTimer = t;
            searchThread = &s;
            duration = d;

            this->commandRun();
        }
    };

    TimerPool::element_type thisTimer;

    Timer (const Timer&) = delete;
    Timer& operator = (const Timer&) = delete;

    static TimerPool::element_type empty() { return timerPool.empty(); }

public:
    Timer () : thisTimer(empty()) {}
   ~Timer () { cancel(); }

    void start(SearchThread& searchThread, duration_t duration) {
        cancel();

        //zero duration means no timer
        if (duration != duration_t::zero()) {
            thisTimer = timerPool.acquire();
            thisTimer->start(thisTimer, searchThread, duration);
        }
    }

    void cancel() {
        if (thisTimer != empty()) {
            //inform the timer that it is free now
            thisTimer->commandStop();

            //now we can forget about this timer
            thisTimer = empty();
        }
    }

};

#endif
