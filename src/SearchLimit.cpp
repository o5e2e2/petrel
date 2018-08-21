#include "SearchLimit.hpp"
#include "PositionFen.hpp"

SearchLimit::SearchLimit () {
    movetime = time[My] = time[Op] = inc[My] = inc[Op] = Duration::zero();
    ponder = infinite = perft = divide = false;
    nodes = NodeCountMax; //almost no limit

    movestogo = 0;
    depth = 0;
    mate = 0;
}

Duration SearchLimit::getThinkingTime() const {
    if (movetime != Duration::zero()) { return movetime; }

    auto moves = movestogo ? movestogo : 60;
    auto average = (time[My] + (moves-1)*inc[My]) / moves;

    return std::min(time[My], average);
}
