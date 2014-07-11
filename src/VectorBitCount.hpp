#ifndef VECTOR_BIT_COUNT_HPP
#define VECTOR_BIT_COUNT_HPP

#include "Vector.hpp"

class VectorBitCount {
public:
    typedef __m128i _t;

private:
    _t nibble_count, lo_nibble_mask, empty_mask;

public:
    VectorBitCount () {
        FOR_INDEX (Index<0x10>, n) {
            reinterpret_cast<char*>(&lo_nibble_mask)[n] = 0x0f;
            reinterpret_cast<char*>(&empty_mask)[n] = 0;

            index_t c{0}; for (index_t b = n; b; b &= b-1) { c++; }
            reinterpret_cast<char*>(&nibble_count)[n] = static_cast<char>(c);
        }
    }

    _t partial(_t v) const {
        _t lo = v & lo_nibble_mask;
        _t hi = _mm_srli_epi16(v, 4) & lo_nibble_mask;
        lo = _mm_shuffle_epi8(nibble_count, lo);
        hi = _mm_shuffle_epi8(nibble_count, hi);
        lo = _mm_add_epi8(lo, hi);
        return lo;
    }

    index_t final(_t v) const {
        _t lo = _mm_sad_epu8(v, empty_mask);
        _t hi = _mm_unpackhi_epi64(lo, empty_mask);
        lo = _mm_add_epi64(lo, hi);
        return static_cast<index_t>(_mm_cvtsi128_si32(lo));
    }

    index_t operator() (_t v) const {
        return final(partial(v));
    }
};

#endif
