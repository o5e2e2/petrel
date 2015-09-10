#ifndef BIT_OPS_128_HPP
#define BIT_OPS_128_HPP

#include "bitops.hpp"

#if defined __SSSE3__ || defined _M_X64 || (defined _MSC_VER && defined _M_IX86_FP && _M_IX86_FP >= 2)
#   include <tmmintrin.h>
#else
#   error The code requiers to enable compiler to use SSSE3 extension
#endif

#if !defined PLATFORM_64
    INLINE __int64 _mm_cvtsi128_si64(__m128i v) {
        auto lo = _mm_cvtsi128_si32(v);
        auto hi = _mm_cvtsi128_si32(_mm_shuffle_epi32(v, _MM_SHUFFLE(1, 1, 1, 1)));
        return ::combine(lo, hi);
    }

    INLINE __m128i _mm_cvtsi64_si128(__int64 b) {
        return _mm_unpacklo_epi32(_mm_cvtsi32_si128(::lo(b)), _mm_cvtsi32_si128(::hi(b)));
    }
#endif

INLINE __m128i combine(__int64 lo, __int64 hi) {
    return _mm_unpacklo_epi64( _mm_cvtsi64_si128(lo), _mm_cvtsi64_si128(hi) );
}

INLINE __m128i _mm_swap_epi64(__m128i v) {
    return _mm_shuffle_epi32(v, _MM_SHUFFLE(1, 0, 3, 2));
}

#endif
