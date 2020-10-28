#include "NodeAlphaBeta.hpp"
#include "SearchControl.hpp"

NodeControl NodeAlphaBeta::visit(Square from, Square to) {
    auto& p = static_cast<NodeAlphaBeta&>(parent);

    playMove(p, from, to, getZobrist());

    if (getMovesCount() == 0 || draft == 0) {
        bestScore = getStaticEval();
    }
    else {
        bestScore = Score::Minimum;
        NodeControl result = NodeAlphaBeta(*this).visitChildren();

        //the search is interrupted so exit immediately
        if (result == NodeControl::Abort && control.isAborted()) {
            return NodeControl::Abort;
        }
    }

    if (-bestScore > p.bestScore) {
        p.bestScore = -bestScore;
        p.bestMove = p.createMove(from, to);

        if (p.alpha < p.bestScore) {
            p.alpha = p.bestScore;

            if (p.alpha >= p.beta) {
                //beta cut off
                return NodeControl::Abort;
            }
        }
    }

    return NodeControl::Continue;
}
