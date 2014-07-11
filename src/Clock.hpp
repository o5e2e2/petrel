#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

typedef std::chrono::milliseconds duration_t;

class Clock {
    typedef std::chrono::steady_clock _t;
    std::chrono::time_point<_t> start;

public:
    Clock () { restart(); }
    void restart() { start = _t::now(); }

    duration_t read() {
        return std::chrono::duration_cast<duration_t>(_t::now() - start);
    }
};

#endif
