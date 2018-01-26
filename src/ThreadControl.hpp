#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <thread>
#include <condition_variable>
#include "SpinLock.hpp"

class ThreadControl {
public:
    typedef int Sequence;

private:
    SpinLock statusLock;

    std::condition_variable_any statusChanged;

    enum Status { Ready, Run, Abort };
    volatile Status status;
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
    virtual ~ThreadControl() {}

    bool isReady()   const { return isStatus(Ready); }
    bool isStopped() const { return isStatus(Abort); }

    Sequence commandRun() { return signalSequence(Ready, Run); }
    void commandStop() { signal(Run, Abort); }
    void commandStop(Sequence seq) { signal(seq, Run, Abort); }

    void stop(Sequence seq) { commandStop(seq); wait(Ready); }
};

#endif
