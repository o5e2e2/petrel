#ifndef NODE_HPP
#define NODE_HPP

#include "PositionMoves.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

class SearchControl;

class Node : public PositionMoves {
protected:
    Node& parent; //virtual
    SearchControl& control;

    Node (Node& p) : PositionMoves(), parent(p), control(p.control) {}

public:
    Node (const PositionMoves& pm, SearchControl& c) : PositionMoves(pm), parent(*this), control(c) {}
    virtual ~Node() {}

    virtual bool visit(Square, Square) { return false; };
    virtual bool visitChildren();
};

#endif
