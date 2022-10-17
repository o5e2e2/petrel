#ifndef VECTOR_BIT_COUNT_HPP
#define VECTOR_BIT_COUNT_HPP

#include "bitops128.hpp"
#include "VectorOf.hpp"

class VectorBitCount {
public:
    typedef __m128i _t;

private:
    const union {
        char b[16];
        _t v;
    } nibbleCount = {{ 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, }};

    const _t& emptyMask = ::vectorOfAll[0];
    const _t& nibbleMask = ::vectorOfAll[0x0f];

public:
    _t bytes(_t v) const {
        _t lo = v & nibbleMask;
        _t hi = _mm_srli_epi16(v, 4) & nibbleMask;
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
        return total(bytes(v));
    }
};

extern const VectorBitCount bitCount;

#endif
