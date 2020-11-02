#ifndef NODE_ALPHA_BETA_HPP
#define NODE_ALPHA_BETA_HPP

#include "Node.hpp"
#include "Score.hpp"

class NodeAlphaBeta : public Node {
public:
    Score alpha = Score::Minimum;
    Score beta = Score::Maximum;
    Score bestScore = Score::Minimum;

public:
    NodeAlphaBeta (NodeAlphaBeta& n, ply_t r = 1) : Node(n, r), alpha{-n.beta}, beta{-n.alpha} {}
    NodeAlphaBeta (const PositionMoves& p, SearchControl& c, ply_t d) : Node(p, c, d) {}

    NodeControl visit(Square from, Square to);
};

#endif
