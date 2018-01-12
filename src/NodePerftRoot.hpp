#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "NodeDeep.hpp"

class NodePerftRoot : public NodeDeep {
    const bool isDivide;
    bool searchIteration();
    bool iterativeDeepening();

public:
    NodePerftRoot (const PositionMoves& p, SearchControl& c, depth_t d, bool b) : NodeDeep(p, c, d), isDivide(b) {}
    bool visitChildren() override;
};

#endif
