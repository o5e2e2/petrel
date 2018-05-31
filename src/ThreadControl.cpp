#include <thread>
#include "ThreadControl.hpp"

namespace {
    using Sequence = ThreadControl::Sequence;
    Sequence& operator++ (Sequence& seq) {
        seq = static_cast<Sequence>( static_cast< std::underlying_type_t<Sequence> >(seq)+1 );
        if (seq == Sequence::None) {
            //wrap around Sequence::None
            seq = static_cast<Sequence>( static_cast< std::underlying_type_t<Sequence> >(Sequence::None)+1 );
        }
        return seq;
    }
}

ThreadControl::ThreadControl () : status{Status::Ready}, sequence{Sequence::None} {
    auto infiniteLoop = [this] {
        for (;;) {
            wait(Status::Run);
            this->thread_body();
            signal(Status::Ready);
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
ThreadControl::Sequence ThreadControl::signalSequence(Status to, Condition condition) {
    Sequence result;

    {
        std::unique_lock<decltype(statusLock)> lock{statusLock};

        if (!condition()) {
            return Sequence::None;
        }

        ++sequence;
        result = sequence;
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

void ThreadControl::signal(Sequence seq, Status from, Status to) {
    if (seq == Sequence::None) {
        return;
    }
    signal(to, [this, seq, from]() { return seq == sequence && isStatus(from); });
}

ThreadControl::Sequence ThreadControl::signalSequence(Status from, Status to) {
    return signalSequence(to, [this, from]() { return isStatus(from); });
}

void ThreadControl::wait(Status to) {
    wait([this, to] { return isStatus(to); });
}
