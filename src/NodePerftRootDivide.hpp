#ifndef NODE_PERFT_ROOT_DIVIDE_HPP
#define NODE_PERFT_ROOT_DIVIDE_HPP

#include "Node.hpp"

class NodePerftRoot;

class NodePerftRootDivide : public Node {
    index_t moveCount = 0;

public:
    NodePerftRootDivide(NodePerftRoot&);
    bool visit(Square from, Square to) override;
};

#endif
