#ifndef TIME_POINT_HPP
#define TIME_POINT_HPP

#include "Duration.hpp"

class TimePoint {
    typedef std::chrono::steady_clock clock_type;
    std::chrono::time_point<clock_type> start;

public:
    TimePoint () : start( clock_type::now() ) {}

    Duration getDuration() const {
        return duration_cast<Duration>(clock_type::now() - start);
    }
};

#endif
