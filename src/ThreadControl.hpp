#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

class ThreadControl {
    enum Status { Ready, Run, Stop };
    volatile Status status;

    std::mutex statusChanging;
    std::condition_variable statusChanged;

    typedef std::unique_lock<decltype(statusChanging)> StatusLock;

    bool isStatus(Status To) const {
        return status == To;
    }

    void signal(Status To) {
        if (!isStatus(To)) {
            StatusLock statusLock(statusChanging);
            status = To;
            statusLock.unlock();
            statusChanged.notify_all();
        }
    }

    void signal(Status From, Status To) {
        if (isStatus(From)) {
            StatusLock statusLock(statusChanging);
            if (isStatus(From)) {
                status = To;
            }
            statusLock.unlock();
            statusChanged.notify_all();
        }
    }

    void wait(Status To) {
        if (!isStatus(To)) {
            StatusLock statusLock(statusChanging);
            statusChanged.wait(statusLock, [this, To] { return isStatus(To); });
        }
    }

    ThreadControl (const ThreadControl&) = delete;
    ThreadControl& operator = (const ThreadControl&) = delete;

    virtual void thread_body() = 0;

public:
    ThreadControl () : status{Ready} {
        auto infinite_loop = [this] {
            for (;;) {
                wait(Run);
                this->thread_body();
                signal(Ready);
            }
        };
        std::thread(infinite_loop).detach();
    }

    virtual ~ThreadControl() {}

    bool isReady()   const { return isStatus(Ready); }
    bool isStopped() const { return isStatus(Stop); }

    void commandRun()  { signal(Ready, Run); }
    void commandStop() { signal(Run, Stop); }

    void waitReady() { wait(Ready); }
    void waitStop()  { wait(Stop); }
};

#endif
