#include <thread>
#include "Timer.hpp"

void TimerThread::thread_body() {
    std::this_thread::sleep_for(duration);
    thread->stop(threadId);
    pool->release(std::move(handle));
}

void Timer::start(Duration duration, ThreadControl& thread, ThreadControl::ThreadId threadId) {
    //zero duration means no timer
    if (threadId == decltype(threadId)::None || duration == Duration::zero()) {
        return;
    }

    auto handle = acquire();
    auto& timer = fetch(handle);
    assert (timer.isReady());

    timer.pool = this;
    timer.handle = handle;
    timer.thread = &thread;
    timer.threadId = threadId;
    timer.duration = duration;

    timer.start();
}
