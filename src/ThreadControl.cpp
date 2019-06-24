#include <thread>
#include "ThreadControl.hpp"

namespace {
    using Id = ThreadControl::TaskId;
    Id& operator++ (Id& id) {
        id = static_cast<Id>( static_cast< std::underlying_type_t<Id> >(id)+1 );
        if (id == Id::None) {
            //wrap around Id::None
            id = static_cast<Id>( static_cast< std::underlying_type_t<Id> >(Id::None)+1 );
        }
        return id;
    }
}

ThreadControl::ThreadControl () : status{Status::Idle}, taskId{TaskId::None} {
    auto infiniteLoop = [this] {
        for (;;) {
            wait(Status::Working);
            this->run();
            signal(Status::Idle);
        }
    };
    std::thread(infiniteLoop).detach();
}

template <typename Condition>
void ThreadControl::wait(Condition condition) {
    std::unique_lock<decltype(statusLock)> lock{statusLock};

    if (!condition()) {
        statusChanged.wait(lock, condition);
    }
}

template <typename Condition>
void ThreadControl::signal(Status to, Condition condition) {
    {
        std::unique_lock<decltype(statusLock)> lock{statusLock};

        if (!condition()) {
            return;
        }

        status = to;
    }

    statusChanged.notify_all();
}

template <typename Condition>
ThreadControl::TaskId ThreadControl::signalSequence(Status to, Condition condition) {
    TaskId result;

    {
        std::unique_lock<decltype(statusLock)> lock{statusLock};

        if (!condition()) {
            return TaskId::None;
        }

        ++taskId;
        result = taskId;
        status = to;
    }

    statusChanged.notify_all();
    return result;
}

void ThreadControl::signal(Status to) {
    signal(to, [this, to]() { return !isStatus(to); });
}

void ThreadControl::signal(Status from, Status to) {
    signal(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::signal(TaskId id, Status from, Status to) {
    if (id == TaskId::None) {
        return;
    }
    signal(to, [this, id, from]() { return id == this->taskId && isStatus(from); });
}

ThreadControl::TaskId ThreadControl::signalSequence(Status from, Status to) {
    return signalSequence(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::wait(Status to) {
    wait([this, to] { return isStatus(to); });
}
