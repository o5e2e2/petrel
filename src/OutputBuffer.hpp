#ifndef OUTPUT_BUFFER_HPP
#define OUTPUT_BUFFER_HPP

#include "io.hpp"
#include <sstream>

class OutputBuffer : public std::ostringstream {
    std::ostream& out;
public:
    OutputBuffer (std::ostream& o) : std::ostringstream{}, out(o) {}
    OutputBuffer (std::ostream* o) : OutputBuffer(*o) {}
   ~OutputBuffer () { out << str() << std::flush; }
};

#endif
