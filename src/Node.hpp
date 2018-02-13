#ifndef NODE_HPP
#define NODE_HPP

#include "PositionMoves.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

class SearchControl;

class Node : public PositionMoves {
public:
    Node& parent; //virtual
    SearchControl& control;

protected:
    Node (Node& p);
    Node (const PositionMoves& p, SearchControl& c);
    Node (const PositionMoves& p, Node& n);

public:
    virtual ~Node() {}
    virtual bool visit(Square, Square) { return false; };
    virtual bool visitChildren();
};

#endif
