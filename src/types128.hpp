#ifndef TYPES_128_HPP
#define TYPES_128_HPP

#include "types.hpp"
#include <tmmintrin.h>

typedef __m128i i128_t;
typedef u8_t  u8x16_t __attribute__((__vector_size__(16), __aligned__(16)));
typedef u64_t u64x2_t __attribute__((__vector_size__(16), __aligned__(16)));

#endif
