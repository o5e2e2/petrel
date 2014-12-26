#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

struct SearchInfo {
    node_count_t nodes;
    node_count_t perft;
    Clock clock;
    Move bestmove;
    Move currmove;
    index_t currmovenumber;
    depth_t depth;

    void clear() {
        nodes = 0;
        clock.restart();
    }

};

#endif
