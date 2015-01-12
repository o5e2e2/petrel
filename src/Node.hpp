#ifndef NODE_HPP
#define NODE_HPP

#include "typedefs.hpp"

class SearchControl;
class Position;

class Node {
protected:
    SearchControl& control;
    depth_t draft; //remaining distance to frontier

public:
    Node (const Node& parent);
    Node (SearchControl&, depth_t depth);

    virtual bool operator() (const Position&) = 0;
    virtual ~Node() {}
};

class NodePerft : public Node {
public:
    NodePerft (SearchControl& c, depth_t depth) : Node{c, depth} {}
    NodePerft (const Node& parent) : Node{parent} {}
    bool operator() (const Position&) override;
};

class NodePerftDivide : public NodePerft {
public:
    NodePerftDivide (SearchControl& c, depth_t depth) : NodePerft{c, depth} {}
    NodePerftDivide (const Node& parent) : NodePerft{parent} {}
    bool operator() (const Position&) override;
};

class NodePerftRoot : public NodePerft {
public:
    NodePerftRoot (SearchControl& c, depth_t depth) : NodePerft{c, depth} {}
    bool operator() (const Position&) override;
};

class NodePerftDivideRoot : public NodePerftDivide {
public:
    NodePerftDivideRoot (SearchControl& c, depth_t depth) : NodePerftDivide{c, depth} {}
    bool operator() (const Position&) override;
};

#endif
