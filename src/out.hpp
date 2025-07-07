#ifndef OUT_HPP
#define OUT_HPP

#include <iostream>

namespace out {
    using std::ostream;
    typedef ostream::char_type char_type;

    ostream& space(ostream&, unsigned, char_type c = ' ');
}

#endif
