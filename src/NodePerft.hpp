#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "Node.hpp"

class NodePerft : public Node {
public:
    node_count_t perft = 0;

protected:
    void updateParentPerft() {
        assert (&parent != this);
        auto& p = static_cast<NodePerft&>(parent);
        p.perft += perft;
        perft = 0;
    }

public:
    NodePerft (NodePerft& n, ply_t r) : Node(n, r) {}
    NodePerft (const PositionMoves& p, SearchControl& c, ply_t d) : Node(p, c, d) {}
    NodeControl visit(Square from, Square to) override;
};

#endif
