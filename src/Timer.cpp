#include <thread>
#include "Timer.hpp"

void TimerThread::run() {
    std::this_thread::sleep_for(duration);
    thread->abort(runId);
    pool->release(std::move(handle));
}

void Timer::start(Duration duration, ThreadControl& thread, ThreadControl::RunId runId) {
    //zero duration means no timer
    if (runId == decltype(runId)::None || duration == Duration::zero()) {
        return;
    }

    auto handle = acquire();
    auto& timer = fetch(handle);
    assert (timer.isReady());

    timer.pool = this;
    timer.handle = handle;
    timer.thread = &thread;
    timer.runId = runId;
    timer.duration = duration;

    timer.start();
}
