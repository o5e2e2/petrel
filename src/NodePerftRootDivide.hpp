#ifndef NODE_PERFT_ROOT_DIVIDE_HPP
#define NODE_PERFT_ROOT_DIVIDE_HPP

#include "Node.hpp"

class NodePerftRootDivide : public Node {
    depth_t draft;
public:
    NodePerftRootDivide(const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c), draft(d) {}
    bool visit(Square from, Square to) override;
};

#endif
