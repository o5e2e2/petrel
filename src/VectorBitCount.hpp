#ifndef VECTOR_BIT_COUNT_HPP
#define VECTOR_BIT_COUNT_HPP

#include "bitops128.hpp"
#include "VectorOf.hpp"

class VectorBitCount {
public:
    typedef __m128i _t;

    typedef union {
        char b[16];
        _t v;
    } Vector;

private:
    const _t& emptyMask = ::vectorOfAll[0];
    const _t& lowNibbleMask = ::vectorOfAll[0x0f];
    const Vector nibbleCount = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,};

public:
    _t perByte(_t v) const {
        _t lo = v & lowNibbleMask;
        _t hi = _mm_srli_epi16(v, 4) & lowNibbleMask;
        lo = _mm_shuffle_epi8(nibbleCount.v, lo);
        hi = _mm_shuffle_epi8(nibbleCount.v, hi);
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
