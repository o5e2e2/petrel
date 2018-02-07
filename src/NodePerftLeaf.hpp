#ifndef NODE_PERFT_LEAF_HPP
#define NODE_PERFT_LEAF_HPP

#include "Node.hpp"
#include "NodePerft.hpp"

class NodePerftLeaf : public Node {
public:
    NodePerftLeaf (NodePerft& p) : Node(p) {}
    bool visit(Square from, Square to) override;
};

#endif
