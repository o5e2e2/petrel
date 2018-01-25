#ifndef NODE_PERFT_TT_HPP
#define NODE_PERFT_TT_HPP

#include "NodePerft.hpp"

class NodePerftTT : public NodePerft {
public:
    NodePerftTT (Node& p, depth_t d) : NodePerft(p, d) {}
    bool visit(Square from, Square to) override;
};

#endif
