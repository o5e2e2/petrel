#ifndef VECTOR_OF_HPP
#define VECTOR_OF_HPP

#include "Vector.hpp"
#include "Index.hpp"

class VectorOf : Index<0x100>::static_array<__m128i> {
    static value_type vectorOfAll(index_type i) { return _mm_shuffle_epi8(_mm_cvtsi32_si128(static_cast<int>(i)), _mm_setzero_si128()); }

public:
    VectorOf () { FOR_INDEX(index_type, i) { static_array::operator[](i) = vectorOfAll(i); } }
    const_reference operator[] (index_type i) const { return static_array::operator[](i); }
    const_reference operator[] (BitRank b) const { return operator[] (index_type{static_cast<BitRank::_t>(b)}); }
};

extern const VectorOf vectorOfAll;

#endif
