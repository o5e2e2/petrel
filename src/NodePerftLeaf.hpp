#ifndef NODE_PERFT_LEAF_HPP
#define NODE_PERFT_LEAF_HPP

#include "Node.hpp"

class NodePerft;

class NodePerftLeaf : public Node {
public:
    NodePerftLeaf (NodePerft&);
    bool visit(Square from, Square to) override;
};

#endif
