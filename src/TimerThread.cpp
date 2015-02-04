#include "TimerThread.hpp"

void TimerThread::thread_body() {
    std::this_thread::sleep_for(duration);

    //TODO: protect with mutex
    if (!this->isStopped()) {
        //signal timeout only if the timer is still active
        slaveThread->commandStop();

        //wait for release
        this->waitStop();
    }

    //inform the pool about idle timer
    ::timerPool.release(std::move(thisTimer));
}

void TimerThread::start(TimerPool::_t t, ThreadControl& s, Clock::_t d) {
    thisTimer = t;
    slaveThread = &s;
    duration = d;

    this->commandRun();
}
