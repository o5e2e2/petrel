#ifndef BIT_ARRAY_128_HPP
#define BIT_ARRAY_128_HPP

#include "bitops128.hpp"
#include "BitArray.hpp"

template <>
constexpr BitArrayBase<__m128i>::BitArrayBase () : _v{0,0} {}

template <>
INLINE bool BitArrayBase<__m128i>::operator == (BitArrayBase<__m128i> a) {
    return _mm_movemask_epi8( _mm_cmpeq_epi8(_v, a._v) ) == 0xffffu;
}

#endif
