#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <condition_variable>
#include "SpinLock.hpp"

class ThreadControl {
public:
    enum class Sequence : unsigned { None };

private:
    enum class Status { Ready, Run, Abort };

    SpinLock statusLock;
    std::condition_variable_any statusChanged;
    Status status;

    Sequence sequence;

    bool isStatus(Status to) const { return status == to; }

    template <typename Condition> void wait(Condition);
    template <typename Condition> void signal(Status, Condition);
    template <typename Condition> Sequence signalSequence(Status,  Condition);

    void wait(Status to);
    void signal(Status to);
    void signal(Status from, Status to);
    void signal(Sequence seq, Status from, Status to);
    Sequence signalSequence(Status from, Status to);

    virtual void thread_body() = 0;

public:
    ThreadControl ();
    virtual ~ThreadControl() = default;

    bool isReady()   const { return isStatus(Status::Ready); }
    bool isStopped() const { return isStatus(Status::Abort); }

    Sequence start() { return signalSequence(Status::Ready, Status::Run); }
    void stop(Sequence seq) { signal(seq, Status::Run, Status::Abort); wait(Status::Ready); }
};

#endif
