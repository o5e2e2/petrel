#ifndef NODE_HPP
#define NODE_HPP

#include "Score.hpp"
#include "NodeControl.hpp"
#include "PositionMoves.hpp"

class SearchControl;

class Node : public PositionMoves {
protected:
    Node& parent; //virtual
    SearchControl& control;

public:
    ply_t draft; //remaining distance to leaves

protected:
    Node (Node& n, ply_t r = 1);
    Node (const PositionMoves& p, SearchControl& c, ply_t d) : PositionMoves{p}, parent{*this}, control{c}, draft{d} {}

public:
    virtual ~Node();
    virtual NodeControl beforeVisit();
    virtual NodeControl visit(Square, Square) { return NodeControl::Continue; }
    virtual NodeControl visitChildren();
};

#endif
