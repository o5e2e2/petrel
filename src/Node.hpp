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
    Node (Node&);
    Node (const PositionMoves&, SearchControl&);
    Node (const PositionMoves&, Node&);

public:
    virtual ~Node() {}
    virtual bool visit(Square, Square) { return false; };
    virtual bool visitChildren();
};

#endif
