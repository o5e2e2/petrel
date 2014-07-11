#ifndef NODE_HPP
#define NODE_HPP

#include "typedefs.hpp"

class Position;

class Node {
public:
    depth_t draft; //remaining distance to frontier
    node_quota_t nodesRemaining; //number of nodes before checking for terminate
    bool checkQuota();

public:
    Node () = default;
    Node (const Node& parent);
    Node (depth_t depth);

    virtual bool operator() (const Position&) = 0;
    virtual ~Node() {}
};

class NodePerft : public Node {
public:
    node_count_t perftNodes;
public:
    NodePerft (depth_t depth) : Node{depth}, perftNodes(0) {}
    NodePerft (const Node& parent) : Node{parent}, perftNodes(0) {}
    bool operator() (const Position&) override;
};

class NodePerftDivide : public NodePerft {
public:
    NodePerftDivide (depth_t depth) : NodePerft{depth} {}
    NodePerftDivide (const Node& parent) : NodePerft{parent} {}
    bool operator() (const Position&) override;
};

class NodePerftRoot : public NodePerft {
public:
    NodePerftRoot (depth_t depth) : NodePerft{depth} {}
    bool operator() (const Position&) override;
};

class NodePerftDivideRoot : public NodePerftDivide {
public:
    NodePerftDivideRoot (depth_t depth) : NodePerftDivide{depth} {}
    bool operator() (const Position&) override;
};

#endif
