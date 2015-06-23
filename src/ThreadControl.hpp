#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <thread>
#include <condition_variable>
#include "SpinLock.hpp"

class ThreadControl {
public:
    typedef int _t;

private:
    SpinLock statusLock;
    typedef SpinLock::Guard Lock;

    std::condition_variable_any statusChanged;

    enum Status { Ready, Run, Abort };
    volatile Status status;
    _t sequence;

    bool isStatus(Status to) const { return status == to; }

    template <typename Condition> void wait(Condition);
    template <typename Condition> void signal(Status, Condition);
    template <typename Condition> _t   signalSequence(Status,  Condition);

    void wait(Status to);
    void signal(Status to);
    void signal(Status from, Status to);
    void signal(_t seq, Status from, Status to);
    _t   signalSequence(Status from, Status to);

    virtual void thread_body() = 0;

public:
    ThreadControl ();
    virtual ~ThreadControl() {}

    bool isReady()   const { return isStatus(Ready); }
    bool isStopped() const { return isStatus(Abort); }

    _t   commandRun() { return signalSequence(Ready, Run); }
    void commandStop() { signal(Run, Abort); }
    void commandStop(_t seq) { signal(seq, Run, Abort); }

    void waitReady() { wait(Ready); }
};

#endif
