#ifndef VECTOR_PI_SINGLE_HPP
#define VECTOR_PI_SINGLE_HPP

#include "typedefs.hpp"
#include "bitops128.hpp"
#include "VectorOf.hpp"

class VectorPiSingle {
    typedef i128_t _t;

    union _u {
        Pi::arrayOf<unsigned char> _a;
        _t _m;
    };

    Pi::arrayOf<_u> _v;

public:
    constexpr VectorPiSingle () :
    _v {{
        {{0xff,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}},
        {{0,0xff,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}},
        {{0,0,0xff,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}},
        {{0,0,0,0xff, 0,0,0,0, 0,0,0,0, 0,0,0,0}},

        {{0,0,0,0, 0xff,0,0,0, 0,0,0,0, 0,0,0,0}},
        {{0,0,0,0, 0,0xff,0,0, 0,0,0,0, 0,0,0,0}},
        {{0,0,0,0, 0,0,0xff,0, 0,0,0,0, 0,0,0,0}},
        {{0,0,0,0, 0,0,0,0xff, 0,0,0,0, 0,0,0,0}},

        {{0,0,0,0, 0,0,0,0, 0xff,0,0,0, 0,0,0,0}},
        {{0,0,0,0, 0,0,0,0, 0,0xff,0,0, 0,0,0,0}},
        {{0,0,0,0, 0,0,0,0, 0,0,0xff,0, 0,0,0,0}},
        {{0,0,0,0, 0,0,0,0, 0,0,0,0xff, 0,0,0,0}},

        {{0,0,0,0, 0,0,0,0, 0,0,0,0, 0xff,0,0,0}},
        {{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0xff,0,0}},
        {{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0xff,0}},
        {{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0xff}},
    }}
    {}

    constexpr const _t& operator[] (Pi pi) const { return _v[pi]._m; }
};

extern const VectorPiSingle vectorPiSingle;

#endif
