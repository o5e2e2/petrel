#include "SearchLimit.hpp"
#include "PositionFen.hpp"

Duration SearchLimit::getThinkingTime() const {
    if (movetime != Duration::zero()) { return movetime; }

    auto moves = movestogo ? movestogo : 60;
    auto average = (time[My] + (moves-1)*inc[My]) / moves;

    return std::min(time[My], average);
}
