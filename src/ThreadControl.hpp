#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

class ThreadControl {
    std::mutex status_mutex;
    std::condition_variable status_wait;

    enum Status { Ready, Run, Stop };
    volatile Status status;

    void signal(Status To) {
        std::unique_lock<decltype(status_mutex)> lock{status_mutex};
        status = To;
        lock.unlock();
        status_wait.notify_all();
    }

    void signal(Status From, Status To) {
        if (status == From) {
            std::unique_lock<decltype(status_mutex)> lock{status_mutex};
            if (status == From) {
                status = To;
                lock.unlock();
                status_wait.notify_all();
            }
        }
    }

    void wait(Status To) {
        if (status != To) {
            std::unique_lock<decltype(status_mutex)> lock{status_mutex};
            status_wait.wait(lock, [this, To] { return status == To; });
        }
    }

    ThreadControl (const ThreadControl&) = delete;
    ThreadControl& operator = (const ThreadControl&) = delete;

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

    virtual void thread_body() = 0;

    bool isReady() const { return status == Ready; }
    bool isStopped() const { return status == Stop; }

    void commandRun() { signal(Ready, Run); }
    void commandStop() { signal(Run, Stop); }

    void waitReady() { wait(Ready); }
    void waitStop() { wait(Stop); }
};

#endif
