#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "Node.hpp"

class NodePerft : public Node {
public:
    node_count_t perft = 0;
    depth_t draft; //remaining depth to leaves

protected:
    void updateParentPerft() {
        assert (&parent != this);
        auto& p = static_cast<NodePerft&>(parent);
        p.perft += perft;
        perft = 0;
    }

public:
    NodePerft (NodePerft& n, depth_t d) : Node(n), draft(d) {}
    NodePerft (const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c), draft{d} {}
    bool visit(Square from, Square to) override;
};

#endif
