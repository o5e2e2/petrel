#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

typedef std::chrono::milliseconds duration_t;

class Clock {
    typedef std::chrono::steady_clock clock_t;
    std::chrono::time_point<clock_t> start;

public:
    Clock () { restart(); }
    void restart() { start = clock_t::now(); }

    duration_t read() {
        return std::chrono::duration_cast<duration_t>(clock_t::now() - start);
    }
};

#endif
