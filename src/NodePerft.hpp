#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "Node.hpp"

class NodePerft : public Node {
    depth_t draft;
public:
    NodePerft (Node& p, depth_t d) : Node(p), draft(d) {}
    bool visit(Square from, Square to) override;
};

#endif
