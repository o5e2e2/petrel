#ifndef OUTPUT_BUFFER_HPP
#define OUTPUT_BUFFER_HPP

#include "io.hpp"

template<typename Lock>
class OutputBuffer : public std::ostringstream {
    io::ostream& out;
    Lock& outLock;
public:
    OutputBuffer (io::ostream& o, Lock& l) : std::ostringstream{}, out(o), outLock(l) {}
   ~OutputBuffer () { outLock.lock(); out << str() << std::flush; outLock.unlock(); }
};

#endif
