#ifndef NODE_PERFT_TT_HPP
#define NODE_PERFT_TT_HPP

#include "NodePerft.hpp"

class NodePerftTT : public NodePerft {
public:
    NodePerftTT (NodePerft& n) : NodePerft{n} {}
    NodeControl visit(Square from, Square to) override;
};

#endif
