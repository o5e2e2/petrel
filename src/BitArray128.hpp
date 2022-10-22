#ifndef BIT_ARRAY_128_HPP
#define BIT_ARRAY_128_HPP

#include "bitops128.hpp"
#include "BitArray.hpp"

template <>
constexpr BitArrayBase<i128_t>::BitArrayBase () : v{0,0} {}

template <>
INLINE bool BitArrayBase<i128_t>::operator == (BitArrayBase<i128_t> a) {
    return _mm_movemask_epi8( _mm_cmpeq_epi8(v, a.v) ) == 0xffffu;
}

#endif
