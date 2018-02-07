#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "Node.hpp"
#include "NodePerft.hpp"

class NodePerft : public Node {
public:
    node_count_t perft;
    depth_t draft;

protected:
    void count() {
        static_cast<NodePerft&>(parent).perft += perft;
        perft = 0;
    }

public:
    NodePerft (const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c), perft(0), draft(d) {}
    NodePerft (NodePerft& p, depth_t d) : Node(p), perft(0), draft(d) {}
    NodePerft (const PositionMoves& p, NodePerft& n) : Node(p, n), perft(0), draft(n.draft) {}
    bool visit(Square from, Square to) override;
};

#endif
