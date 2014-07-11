#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

typedef std::chrono::milliseconds duration_t;

class Clock {
    typedef std::chrono::steady_clock clock; //TRICK: should be steady_clock
    std::chrono::time_point<clock> start;

public:
    Clock () { restart(); }
    void restart() { start = clock::now(); }

    duration_t read() {
        return std::chrono::duration_cast<duration_t>(clock::now() - start);
    }
};

#endif
