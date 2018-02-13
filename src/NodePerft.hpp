#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "Node.hpp"
#include "NodePerft.hpp"

class NodePerft : public Node {
public:
    node_count_t perft = 0;
    depth_t draft;

protected:
    void updateParentPerft() {
        static_cast<NodePerft&>(parent).perft += perft;
        perft = 0;
    }

public:
    NodePerft (NodePerft& p, depth_t d) : Node(p), draft(d) {}
    NodePerft (const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c), draft(d) {}
    NodePerft (const PositionMoves& p, NodePerft& n) : Node(p, n), draft(n.draft) {}
    bool visit(Square from, Square to) override;
};

#endif
