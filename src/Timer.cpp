#include <thread>
#include "Timer.hpp"

void TimerThread::thread_body() {
    std::this_thread::sleep_for(duration);
    thread->stop(sequence);
    pool->release(std::move(handle));
}

void Timer::start(Duration duration, ThreadControl& thread, ThreadControl::Sequence sequence) {
    //zero duration means no timer
    if (sequence == decltype(sequence)::None || duration == Duration::zero()) {
        return;
    }

    auto handle = acquire();
    auto& timer = fetch(handle);
    assert (timer.isReady());

    timer.pool = this;
    timer.handle = handle;
    timer.thread = &thread;
    timer.sequence = sequence;
    timer.duration = duration;

    timer.start();
}
