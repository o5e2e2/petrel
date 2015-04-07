#ifndef DUAL_BB_HPP
#define DUAL_BB_HPP

#include "bitops128.hpp"
#include "Bb.hpp"

INLINE __m128i combine(Bb lo, Bb hi) {
    return ::combine(static_cast<__int64>(lo), static_cast<__int64>(hi));
}

#endif
