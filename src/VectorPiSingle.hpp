#ifndef VECTOR_PI_SINGLE_HPP
#define VECTOR_PI_SINGLE_HPP

#include "typedefs.hpp"
#include "Vector.hpp"

class VectorPiSingle {
    typedef __m128i _t;
    Pi::static_array<_t> _v;

public:
    VectorPiSingle () {
        _t v = _mm_cvtsi32_si128(0xff);
        FOR_INDEX(Pi, pi) {
            _v[pi] = v;
            v = _mm_slli_si128(v, 1);
        }
    }

    _t operator[] (Pi pi) const {
        return _v[pi];
    }
};

extern const VectorPiSingle vectorPiSingle;

#endif
