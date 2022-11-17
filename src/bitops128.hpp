#ifndef BIT_OPS_128_HPP
#define BIT_OPS_128_HPP

#include <tmmintrin.h>
#include "bitops.hpp"

typedef __m128i i128_t;
typedef u8_t u8x16_t __attribute__((__vector_size__(16), __aligned__(16)));

constexpr i128_t all(u8_t i) { return (i128_t)u8x16_t{ i,i,i,i, i,i,i,i, i,i,i,i, i,i,i,i }; }

#endif
