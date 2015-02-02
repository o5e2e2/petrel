#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

class Clock {
    typedef std::chrono::steady_clock clock_t;
    std::chrono::time_point<clock_t> start;

public:
    Clock () { restart(); }
    void restart() { start = clock_t::now(); }

    typedef std::chrono::milliseconds _t;
    _t read() const {
        return std::chrono::duration_cast<_t>(clock_t::now() - start);
    }
};

#endif
