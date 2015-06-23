#include "ThreadControl.hpp"

ThreadControl::ThreadControl () : status{Ready}, sequence{0} {
    auto infinite_loop = [this] {
        for (;;) {
            wait(Run);
            this->thread_body();
            signal(Ready);
        }
    };
    std::thread(infinite_loop).detach();
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
    if (condition()) {
        bool isChanged{false};

        {
            Lock lock(statusLock);

            if (condition()) {
                status = to;
                isChanged = true;
            }
        }

        if (isChanged) {
            statusChanged.notify_all();
        }
    }
}

template <typename Condition>
ThreadControl::_t ThreadControl::signalSequence(Status to, Condition condition) {
    if (condition()) {
        bool isChanged{false};
        ThreadControl::_t result;

        {
            Lock lock(statusLock);

            if (condition()) {
                ++sequence;
                if (sequence == 0) { sequence = 1; } //wrap around
                result = sequence;

                status = to;
                isChanged = true;
            }
        }

        if (isChanged) {
            statusChanged.notify_all();
            return result;
        }
    }
    return 0;
}

void ThreadControl::signal(Status to) {
    signal(to, [this, to]() { return !isStatus(to); });
}

void ThreadControl::signal(Status from, Status to) {
    signal(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::signal(_t seq, Status from, Status to) {
    if (seq != 0) {
        signal(to, [this, seq, from]() { return sequence == seq && isStatus(from); });
    }
}

ThreadControl::_t ThreadControl::signalSequence(Status from, Status to) {
    return signalSequence(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::wait(Status to) {
    wait([this, to] { return isStatus(to); });
}
