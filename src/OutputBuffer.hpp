#ifndef OUTPUT_BUFFER_HPP
#define OUTPUT_BUFFER_HPP

#include <mutex>
#include "io.hpp"

template<class BasicLockable>
class OutputBuffer : public std::ostringstream {
    io::ostream& out;
    BasicLockable& lock;
    typedef std::lock_guard<decltype(lock)> Guard;
public:
    OutputBuffer (io::ostream& o, BasicLockable& l) : std::ostringstream{}, out(o), lock(l) {}
   ~OutputBuffer () { Guard g{lock}; out << str() << std::flush; }
};

#endif
