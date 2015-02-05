#include "ThreadControl.hpp"

void ThreadControl::signal(Status to) {
    if (!isStatus(to)) {
        bool isChanged{false};

        StatusLock statusLock(statusChanging);

        if (!isStatus(to)) {
            status = to;
            isChanged = true;
        }

        statusLock.unlock();

        if (isChanged) {
            statusChanged.notify_all();
        }
    }
}

void ThreadControl::signal(Status from, Status to) {
    if (isStatus(from)) {
        bool isChanged{false};

        StatusLock statusLock(statusChanging);

        if (isStatus(from)) {
            status = to;
            isChanged = true;
        }

        statusLock.unlock();

        if (isChanged) {
            statusChanged.notify_all();
        }
    }
}

void ThreadControl::signal(sequence_t seq, Status from, Status to) {
    if (seq != 0 && seq == sequence && isStatus(from)) {
        bool isChanged{false};

        StatusLock statusLock(statusChanging);

        if (seq == sequence && isStatus(from)) {
            status = to;
            isChanged = true;
        }

        statusLock.unlock();

        if (isChanged) {
            statusChanged.notify_all();
        }
    }
}

ThreadControl::sequence_t ThreadControl::signalSequence(Status from, Status to) {
    if (isStatus(from)) {
        bool isChanged{false};
        sequence_t seq;

        StatusLock statusLock(statusChanging);

        if (isStatus(from)) {
            status = to;
            isChanged = true;

            if (++sequence == 0) { sequence = 1; } //wrap around
            seq = sequence;
        }

        statusLock.unlock();

        if (isChanged) {
            statusChanged.notify_all();
            return seq;
        }
    }
    return 0;
}

void ThreadControl::wait(Status to) {
    if (!isStatus(to)) {
        StatusLock statusLock(statusChanging);
        statusChanged.wait(statusLock, [this, to] { return isStatus(to); });
    }
}
