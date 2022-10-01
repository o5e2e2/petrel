#include <thread>
#include "Timer.hpp"
#include "typedefs.hpp"

void TimerThread::run() {
    std::this_thread::sleep_for(duration);
    thread->stop(taskId);
    pool->release(std::move(iterator));
}

void Timer::start(Duration duration, ThreadControl& thread, ThreadControl::TaskId taskId) {
    //zero duration means no timer
    if (duration == Duration::zero() || taskId == decltype(taskId)::None || !thread.isTask(taskId)) {
        return;
    }

    auto timerIterator = acquire();
    auto& timer = fetch(timerIterator);
    assert (timer.isIdle());

    timer.pool = this;
    timer.iterator = timerIterator;
    timer.thread = &thread;
    timer.taskId = taskId;
    timer.duration = duration;

    timer.start();
}
