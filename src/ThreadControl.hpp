#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <thread>
#include <mutex>
#include <condition_variable>

#include "SpinLock.hpp"

class ThreadControl {
public:
    typedef int sequence_t;

private:
    enum Status { Ready, Run, Abort };
    volatile Status status;
    sequence_t sequence;

    std::condition_variable_any statusChanged;

    SpinLock statusChanging;
    typedef std::lock_guard<decltype(statusChanging)> StatusLock;

    bool isStatus(Status to) const { return status == to; }
    void wait(Status to);

    void signal(Status to);
    void signal(Status from, Status to);

    sequence_t signalSequence(Status from, Status to);
    void signal(sequence_t seq, Status from, Status to);

    virtual void thread_body() = 0;

public:
    ThreadControl ();
    virtual ~ThreadControl() {}

    bool isReady()   const { return isStatus(Ready); }
    bool isStopped() const { return isStatus(Abort); }

    sequence_t commandRun()  { return signalSequence(Ready, Run); }
    void commandStop(sequence_t seq) { signal(seq, Run, Abort); }
    void commandStop() { signal(Run, Abort); }

    void waitReady() { wait(Ready); }
};

#endif
