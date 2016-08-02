#ifndef TIMER_HPP
#define TIMER_HPP

#include "Clock.hpp"
#include "Pool.hpp"
#include "ThreadControl.hpp"

class Timer : private ThreadControl {  
public:
    typedef ::Pool<Timer> Pool;

private:
    Pool* timerPool;
    Pool::_t timerHandle;
    ThreadControl* slaveThread;
    ThreadControl::_t slaveSequence;
    Clock::_t duration;

    void thread_body() override {
        std::this_thread::sleep_for(duration);
        slaveThread->commandStop(slaveSequence);

        timerPool->release(std::move(timerHandle));
    }

public:
    static ThreadControl::_t run(Pool& timerPool, ThreadControl& slaveThread, Clock::_t duration) {
        assert (slaveThread.isReady());
        ThreadControl::_t slaveSequence = slaveThread.commandRun();

        //zero duration means no timer
        if (slaveSequence && duration != Clock::_t::zero()) {
            Pool::_t timerHandle = timerPool.acquire();
            Timer& timer = timerPool.fetch(timerHandle);

            timer.timerPool = &timerPool;
            timer.timerHandle = timerHandle;
            timer.slaveThread = &slaveThread;
            timer.slaveSequence = slaveSequence;
            timer.duration = duration;

            assert (timer.isReady());
            timer.commandRun();
        }

        return slaveSequence;
    }
};

#endif
