#include <thread>
#include "ThreadControl.hpp"

ThreadControl::ThreadControl () : status{Ready}, sequence{0} {
    auto infiniteLoop = [this] {
        for (;;) {
            wait(Run);
            this->thread_body();
            signal(Ready);
        }
    };
    std::thread(infiniteLoop).detach();
}

template <typename Condition>
void ThreadControl::wait(Condition condition) {
    if (!condition()) {
        std::unique_lock<decltype(statusLock)> lock(statusLock);
        statusChanged.wait(lock, condition);
    }
}

template <typename Condition>
void ThreadControl::signal(Status to, Condition condition) {
    statusLock.lock();
    if (!condition()) {
        statusLock.unlock();
        return;
    }

    status = to;

    statusLock.unlock();
    statusChanged.notify_all();
}

template <typename Condition>
ThreadControl::Sequence ThreadControl::signalSequence(Status to, Condition condition) {
    statusLock.lock();
    if (!condition()) {
        statusLock.unlock();
        return 0;
    }

    ++sequence;
    if (sequence == 0) { sequence = 1; } //wrap around
    Sequence result = sequence;
    status = to;

    statusLock.unlock();
    statusChanged.notify_all();
    return result;
}

void ThreadControl::signal(Status to) {
    signal(to, [this, to]() { return !isStatus(to); });
}

void ThreadControl::signal(Status from, Status to) {
    signal(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::signal(Sequence seq, Status from, Status to) {
    if (seq != 0) {
        signal(to, [this, seq, from]() { return sequence == seq && isStatus(from); });
    }
}

ThreadControl::Sequence ThreadControl::signalSequence(Status from, Status to) {
    return signalSequence(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::wait(Status to) {
    wait([this, to] { return isStatus(to); });
}
