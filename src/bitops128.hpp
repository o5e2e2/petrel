#ifndef BIT_OPS_128_HPP
#define BIT_OPS_128_HPP

#include <tmmintrin.h>
#include "bitops.hpp"

typedef __m128i i128_t;
typedef union { u8_t u8[16]; i128_t i128; } u8x16_t;

#endif
