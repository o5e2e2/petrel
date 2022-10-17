#ifndef SCORE_HPP
#define SCORE_HPP

#include "typedefs.hpp"

typedef i16_t score_t;

enum class Score : score_t {
    None = std::numeric_limits<score_t>::min(),
    Maximum = std::numeric_limits<score_t>::max(),
    Minimum = -Maximum,
    Checkmated = Minimum,
    Draw = 0
};
constexpr Score operator - (Score a) { return static_cast<Score>(-static_cast<score_t>(a)); }
constexpr Score operator - (Score a, Score b) { return static_cast<Score>(static_cast<score_t>(a) - static_cast<score_t>(b)); }

#endif
