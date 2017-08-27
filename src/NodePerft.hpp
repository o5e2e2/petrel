#ifndef NODE_PERFT_HPP
#define NODE_PERFT_HPP

#include "NodeDeep.hpp"

class NodePerft : public NodeDeep {
public:
    NodePerft (NodeDeep& p) : NodeDeep(p) {}
    bool visit(Square from, Square to) override;
};

#endif
