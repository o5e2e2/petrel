#ifndef NODE_DEEP_HPP
#define NODE_DEEP_HPP

#include "typedefs.hpp"
#include "Node.hpp"

class NodeDeep : public Node {
public:
    depth_t draft;
    NodeDeep (const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c), draft(d) {}
    NodeDeep (NodeDeep& p) : Node(p), draft(p.draft-1) {}
};

#endif
