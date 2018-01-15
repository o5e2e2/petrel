#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "Node.hpp"

class NodePerftRoot : public Node {
    bool searchIteration(depth_t);
    bool iterativeDeepening();

public:
    NodePerftRoot (const PositionMoves& p, SearchControl& c) : Node(p, c) {}
    bool visitChildren() override;
};

#endif
