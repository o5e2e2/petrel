#ifndef IO_HPP
#define IO_HPP

#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace io {
    typedef std::ostream::char_type char_type;
    typedef const char_type* literal;

    std::istream& fail_here(std::istream&);
    std::istream& fail_char(std::istream&);
    std::istream& fail_pos(std::istream&, std::streampos);
    std::istream& fail_rewind(std::istream&);

    bool next(std::istream&, io::literal);

    std::ostream& app_version(std::ostream&);
}

#endif
