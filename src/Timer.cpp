#include "Timer.hpp"
#include "TimerThread.hpp"

void Timer::start(ThreadControl& slaveThread, Clock::_t duration) {
    cancel();

    //zero duration means no timer
    if (duration != Clock::_t::zero()) {
        thisTimer = ::timerPool.acquire();
        (*thisTimer).start(thisTimer, slaveThread, duration);
    }
}

void Timer::cancel() {
    if (!::timerPool.empty(thisTimer)) {
        //inform the timer that it is free now
        (*thisTimer).commandStop();

        //now we can forget about this timer
        ::timerPool.clear(std::move(thisTimer));
    }
}
