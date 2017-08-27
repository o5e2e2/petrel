#ifndef NODE_ROOT_HPP
#define NODE_ROOT_HPP

#include "NodeDeep.hpp"

class NodeRoot : public NodeDeep {
    bool searchDepth();
public:
    bool isDivide;
    NodeRoot (const PositionMoves& pm, SearchControl& c, depth_t d, bool b) : NodeDeep(pm, c, d), isDivide(b) {}
    bool visitChildren(Node&) override;
};

#endif
