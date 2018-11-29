#ifndef NODE_HPP
#define NODE_HPP

#include "PositionMoves.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

class SearchControl;

class Node : public PositionMoves {
protected:
    Node& parent; //virtual
    SearchControl& control;

public:
    Score bestScore = Score::Minimum;
    Move bestMove = {};

protected:
    Node (Node& n) : PositionMoves{}, parent{n}, control{n.control} {}
    Node (const PositionMoves& p, SearchControl& c) : PositionMoves{p}, parent{*this}, control{c} {}

public:
    virtual ~Node() = default;
    virtual bool visit(Square, Square) { return false; };
    virtual bool visitChildren();
};

#endif
