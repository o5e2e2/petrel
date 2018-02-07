#ifndef NODE_HPP
#define NODE_HPP

#include "PositionMoves.hpp"
#include "SearchControl.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

class Node : public PositionMoves {
public:
    Node& parent; //virtual
    SearchControl& control;

protected:
    Node (Node& p) : PositionMoves(), parent(p), control(p.control) {}
    Node (const PositionMoves& p, SearchControl& c) : PositionMoves(p), parent(*this), control(c) {}
    Node (const PositionMoves& p, Node& n) : PositionMoves(p), parent(n), control(n.control) {}

public:
    virtual ~Node() {}
    virtual bool visit(Square, Square) { return false; };
    virtual bool visitChildren();
};

#endif
