#ifndef VECTOR_BIT_COUNT_HPP
#define VECTOR_BIT_COUNT_HPP

#include "bitops128.hpp"

class VectorBitCount {
public:
    typedef __m128i _t;

private:
    _t nibbleCount, lowNibbleMask, emptyMask;

public:
    VectorBitCount () {
        FOR_INDEX (Index<0x10>, n) {
            reinterpret_cast<char*>(&lowNibbleMask)[n] = 0x0f;
            reinterpret_cast<char*>(&emptyMask)[n] = 0;

            index_t c{0}; for (Index<0x10>::_t b = n; b; b &= b-1) { c++; }
            reinterpret_cast<char*>(&nibbleCount)[n] = static_cast<char>(c);
        }
    }

    _t perByte(_t v) const {
        _t lo = v & lowNibbleMask;
        _t hi = _mm_srli_epi16(v, 4) & lowNibbleMask;
        lo = _mm_shuffle_epi8(nibbleCount, lo);
        hi = _mm_shuffle_epi8(nibbleCount, hi);
        return _mm_add_epi8(lo, hi);
    }

    index_t total(_t v) const {
        _t lo = _mm_sad_epu8(v, emptyMask);
        _t hi = _mm_unpackhi_epi64(lo, emptyMask);
        lo = _mm_add_epi64(lo, hi);
        return static_cast<index_t>(_mm_cvtsi128_si32(lo));
    }

    index_t operator() (_t v) const {
        return total(perByte(v));
    }
};

extern const VectorBitCount bitCount;

#endif
