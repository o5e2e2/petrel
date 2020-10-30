#ifndef NODE_ALPHA_BETA_HPP
#define NODE_ALPHA_BETA_HPP

#include "Node.hpp"
#include "Score.hpp"

class NodeAlphaBeta : public Node {
public:
    depth_t ply = 0;
    Score alpha = Score::Minimum;
    Score beta = Score::Maximum;
    Score bestScore = Score::Minimum;
    Move bestMove = {};

public:
    NodeAlphaBeta (NodeAlphaBeta& n, depth_t r = 1) : Node(n, r), ply{n.ply+1}, alpha{-n.beta}, beta{-n.alpha} {}
    NodeAlphaBeta (const PositionMoves& p, SearchControl& c, depth_t d) : Node(p, c, d) {}

    NodeControl visit(Square from, Square to);
};

#endif
