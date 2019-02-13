#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <condition_variable>
#include "SpinLock.hpp"

class ThreadControl {
public:
    enum class RunId : unsigned { None };

private:
    /**
     * Ready: the thread is idle
     * Run: the thread is running job with current jobId
     * Abort: the thread is running but going to finish soon and became Ready again
     **/
    enum class Status { Ready, Run, Abort };

    SpinLock statusLock;
    std::condition_variable_any statusChanged;
    Status status;

    RunId runId;

    bool isStatus(Status to) const { return status == to; }

    template <typename Condition> void wait(Condition);
    template <typename Condition> void signal(Status, Condition);
    template <typename Condition> RunId signalSequence(Status,  Condition);

    void wait(Status to);
    void signal(Status to);
    void signal(Status from, Status to);
    void signal(RunId id, Status from, Status to);
    RunId signalSequence(Status from, Status to);

    virtual void run() = 0;

public:
    ThreadControl ();
    virtual ~ThreadControl() = default;

    bool isReady()   const { return isStatus(Status::Ready); }
    bool isRunning() const { return isStatus(Status::Run); }
    bool isStopped() const { return isStatus(Status::Abort); }

    RunId start() { return signalSequence(Status::Ready, Status::Run); }
    void stop() { signal(Status::Run, Status::Abort); wait(Status::Ready); }
    void abort(RunId id) { signal(id, Status::Run, Status::Abort); }
};

#endif
