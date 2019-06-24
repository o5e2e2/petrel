#include <thread>
#include "Timer.hpp"
#include "typedefs.hpp"

void TimerThread::run() {
    std::this_thread::sleep_for(duration);
    thread->stop(taskId);
    pool->release(std::move(handle));
}

void Timer::start(Duration duration, ThreadControl& thread, ThreadControl::TaskId taskId) {
    //zero duration means no timer
    if (taskId == decltype(taskId)::None || duration == Duration::zero()) {
        return;
    }

    auto handle = acquire();
    auto& timer = fetch(handle);
    assert (timer.isIdle());

    timer.pool = this;
    timer.handle = handle;
    timer.thread = &thread;
    timer.taskId = taskId;
    timer.duration = duration;

    timer.start();
}
