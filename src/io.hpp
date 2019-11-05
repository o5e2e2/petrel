#ifndef IO_HPP
#define IO_HPP

#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace io {
    using std::istream;
    using std::ostream;

    typedef ostream::char_type char_type;
    typedef const char_type* czstring;

    istream& fail(istream&);
    istream& fail_char(istream&);
    istream& fail_pos(istream&, std::streampos);
    istream& fail_rewind(istream&);

    bool next(istream&, czstring);
    bool nextNone(istream&);

    ostream& app_version(ostream&);
}

#endif
