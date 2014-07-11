#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "bitops.hpp"
#include "BitArray.hpp"

#if defined __SSSE3__ || defined _M_X64 || (defined _MSC_VER && defined _M_IX86_FP && _M_IX86_FP >= 2)
#   include <tmmintrin.h>
#else
#   error The code requiers to enable compiler to use SSSE3 extension
#endif

template <>
constexpr BitArrayBase<__m128i>::BitArrayBase () : _v{0,0} {}

template <>
inline void BitArrayBase<__m128i>::operator %= (BitArrayBase<__m128i> a) {
    _v = _mm_andnot_si128(a._v, _v);
}

template <>
inline bool BitArrayBase<__m128i>::operator == (BitArrayBase<__m128i> a) {
    return _mm_movemask_epi8( _mm_cmpeq_epi8(_v, a._v) ) == 0xffffu;
}

#endif
