#ifndef NODE_PERFT_ROOT_DIVIDE_HPP
#define NODE_PERFT_ROOT_DIVIDE_HPP

#include "NodeDeep.hpp"

class NodePerftRootDivide : public NodeDeep {
public:
    NodePerftRootDivide(const PositionMoves& p, SearchControl& c, depth_t d) : NodeDeep(p, c, d) {}
    bool visit(Square from, Square to) override;
};

#endif
