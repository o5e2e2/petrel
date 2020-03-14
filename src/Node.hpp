#ifndef NODE_HPP
#define NODE_HPP

#include "PositionMoves.hpp"

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
    virtual Control visit(Square, Square) { return Control::Continue; };
    virtual Control visitChildren();
};

#endif
