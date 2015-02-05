#ifndef THREAD_CONTROL_HPP
#define THREAD_CONTROL_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

class ThreadControl {
public:
    typedef int sequence_t;

private:
    enum Status { Ready, Run, Pause, Abort };
    volatile Status status;

    sequence_t sequence;

    std::mutex statusChanging;
    std::condition_variable statusChanged;

    typedef std::unique_lock<decltype(statusChanging)> StatusLock;

    bool isStatus(Status to) const { return status == to; }

    void signal(Status to);
    void signal(Status from, Status to);

    sequence_t signalSequence(Status from, Status to);
    void signal(sequence_t seq, Status from, Status to);

    void wait(Status to);

    ThreadControl (const ThreadControl&) = delete;
    ThreadControl& operator = (const ThreadControl&) = delete;

    virtual void thread_body() = 0;

public:
    ThreadControl () : status{Ready}, sequence{0} {
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
    bool isStopped() const { return isStatus(Abort); }

    void commandRun()  { signal(Ready, Run); }
    void commandStop() { signal(Abort); }

    sequence_t commandRunSequence()  { return signalSequence(Ready, Run); }
    void commandStop(sequence_t seq) { signal(seq, Run, Abort); }

    void commandPause(sequence_t seq) { signal(seq, Run, Pause); }
    void commandContinue(sequence_t seq) { signal(seq, Pause, Run); }

    void waitReady() { wait(Ready); }
};

#endif
