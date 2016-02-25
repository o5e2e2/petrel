#include "SearchLimit.hpp"

SearchLimit::SearchLimit () {
    clear();
}

void SearchLimit::clear() {
    movetime = time[My] = time[Op] = inc[My] = inc[Op] = Clock::_t::zero();
    ponder = infinite = perft = divide = false;
    nodes = NODE_COUNT_NONE;

    movestogo = 0;
    depth = 0;
    mate = 0;
}

Clock::_t SearchLimit::getThinkingTime() const {
    if (movetime != Clock::_t::zero()) { return movetime; }

    auto moves_to_go = movestogo? movestogo : 40;
    auto average = (time[My] + moves_to_go*inc[My]) / moves_to_go;

    return std::min(time[My], average);
}
