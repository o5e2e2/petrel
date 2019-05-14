#ifndef MILLISECONDS_HPP
#define MILLISECONDS_HPP

#include "io.hpp"
#include "Duration.hpp"

typedef std::chrono::milliseconds Milliseconds; //UCI i/o type

template <typename duration_type>
constexpr auto milliseconds(duration_type duration)
    -> decltype(duration_cast<Milliseconds>(duration).count()) {
    return duration_cast<Milliseconds>(duration).count();
}

template <typename nodes_type, typename duration_type>
constexpr nodes_type nps(nodes_type nodes, duration_type duration) {
    return (nodes * duration_type::period::den) / (static_cast<nodes_type>(duration.count()) * duration_type::period::num);
}

inline io::istream& operator >> (io::istream& in, Duration& duration) {
    unsigned long msecs;
    if (in >> msecs) {
        duration = duration_cast<Duration>(Milliseconds{msecs});
    }
    return in;
}

#endif
