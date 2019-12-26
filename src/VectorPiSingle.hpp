#ifndef VECTOR_PI_SINGLE_HPP
#define VECTOR_PI_SINGLE_HPP

#include "typedefs.hpp"
#include "bitops128.hpp"
#include "VectorOf.hpp"

class VectorPiSingle {
    typedef __m128i _t;

    union _u {
        Pi::array<unsigned char> _a;
        _t _m;
    };

    Pi::array<_u> _v;

public:
    constexpr VectorPiSingle () :
    _v {{
        {0xff,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
        {0,0xff,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
        {0,0,0xff,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
        {0,0,0,0xff, 0,0,0,0, 0,0,0,0, 0,0,0,0},

        {0,0,0,0, 0xff,0,0,0, 0,0,0,0, 0,0,0,0},
        {0,0,0,0, 0,0xff,0,0, 0,0,0,0, 0,0,0,0},
        {0,0,0,0, 0,0,0xff,0, 0,0,0,0, 0,0,0,0},
        {0,0,0,0, 0,0,0,0xff, 0,0,0,0, 0,0,0,0},

        {0,0,0,0, 0,0,0,0, 0xff,0,0,0, 0,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,0xff,0,0, 0,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,0,0xff,0, 0,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,0,0,0xff, 0,0,0,0},

        {0,0,0,0, 0,0,0,0, 0,0,0,0, 0xff,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0xff,0,0},
        {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0xff,0},
        {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0xff},
    }}
    {}

    constexpr const _t& operator[] (Pi pi) const { return _v[pi]._m; }
    constexpr _t except(Pi pi) const { return (*this)[pi] ^ ::vectorOfAll[0xff]; };

};

extern const VectorPiSingle vectorPiSingle;

#endif
