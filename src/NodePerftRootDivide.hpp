#ifndef NODE_PERFT_ROOT_DIVIDE_HPP
#define NODE_PERFT_ROOT_DIVIDE_HPP

#include "NodePerft.hpp"

class NodePerftRoot;

class NodePerftRootDivide : public NodePerft {
    index_t moveCount;

public:
    NodePerftRootDivide(NodePerftRoot&);
    bool visit(Square from, Square to) override;
};

#endif
