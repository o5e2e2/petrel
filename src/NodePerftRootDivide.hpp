#ifndef NODE_PERFT_ROOT_DIVIDE_HPP
#define NODE_PERFT_ROOT_DIVIDE_HPP

#include "NodePerft.hpp"

class NodePerftRoot;

class NodePerftRootDivide : public NodePerft {
    index_t moveCount = 0;

public:
    NodePerftRootDivide(NodePerftRoot&);
    NodeControl visit(Square from, Square to) override;
};

#endif
