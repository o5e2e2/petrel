#ifndef IO_HPP
#define IO_HPP

#include <iostream>

namespace io {
    typedef std::istream::char_type char_type;
    typedef const char_type* literal;

    void fail_here(std::istream&);
    void fail_char(std::istream&);
    void fail_pos(std::istream&, std::streampos);
    void fail_rewind(std::istream&);

    bool next(std::istream&, io::literal);

    std::ostream& app_version(std::ostream&);
}

#endif
