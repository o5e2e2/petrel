#ifndef NODE_PERFT_ROOT_HPP
#define NODE_PERFT_ROOT_HPP

#include "NodePerft.hpp"

class NodePerftRoot : public NodePerft {
    bool searchIteration();
    bool iterativeDeepening();

public:
    NodePerftRoot (const PositionMoves& p, SearchControl& c) : NodePerft(p, c, 1) {}
    bool visitChildren() override;
};

#endif
