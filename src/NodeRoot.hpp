#ifndef NODE_ROOT_HPP
#define NODE_ROOT_HPP

#include "NodeDeep.hpp"

class NodeRoot : public NodeDeep {
    const bool isDivide;
    bool searchIteration();
    bool iterativeDeepening();

public:
    NodeRoot (const PositionMoves& pm, SearchControl& c, depth_t d, bool b) : NodeDeep(pm, c, d), isDivide(b) {}
    bool visitChildren() override;
};

#endif
