#ifndef OUTPUT_BUFFER_HPP
#define OUTPUT_BUFFER_HPP

#include "io.hpp"

template<typename Lock>
class OutputBuffer : public std::ostringstream {
    std::ostream& out;
    Lock& outLock;
public:
    OutputBuffer (std::ostream& o, Lock& l) : std::ostringstream{}, out(o), outLock(l) {}
   ~OutputBuffer () { outLock.lock(); out << str() << std::flush; outLock.unlock(); }
};

#endif
