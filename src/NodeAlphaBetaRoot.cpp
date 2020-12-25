#include "NodeAlphaBetaRoot.hpp"
#include "NodeAlphaBeta.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"

NodeAlphaBetaRoot::NodeAlphaBetaRoot (const SearchLimit& limit, SearchControl& searchControl):
    NodeAlphaBeta(limit.positionMoves, searchControl, 1), depthLimit(limit.depth ? limit.depth : DepthMax)
{}

NodeControl NodeAlphaBetaRoot::searchIteration() {
    RETURN_CONTROL ( NodeAlphaBeta(*this).visitChildren() );
    control.infoDepth(draft, bestScore);
    return NodeControl::Continue;
}

NodeControl NodeAlphaBetaRoot::iterativeDeepening() {
    for (draft = 1; draft <= depthLimit; ++draft) {
        RETURN_CONTROL ( searchIteration() );

        bestScore = Score::None;

        control.nextIteration();
    }

    return NodeControl::Continue;
}

NodeControl NodeAlphaBetaRoot::visitChildren() {
    iterativeDeepening();

    control.bestmove(bestScore);
    return NodeControl::Abort;
}
