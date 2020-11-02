#include "NodeAlphaBetaRoot.hpp"
#include "NodeAlphaBeta.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"

NodeAlphaBetaRoot::NodeAlphaBetaRoot (const SearchLimit& limit, SearchControl& searchControl):
    NodeAlphaBeta(limit.positionMoves, searchControl, limit.depth)
{}

NodeControl NodeAlphaBetaRoot::searchIteration() {
    RETURN_CONTROL ( NodeAlphaBeta(*this).visitChildren() );
    control.infoDepth(draft, bestScore);
    return NodeControl::Continue;
}

NodeControl NodeAlphaBetaRoot::iterativeDeepening() {
    for (draft = 1; draft <= DepthMax; ++draft) {
        RETURN_CONTROL ( searchIteration() );

        bestScore = Score::None;

        control.nextIteration();
    }

    return NodeControl::Continue;
}

NodeControl NodeAlphaBetaRoot::visitChildren() {
    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.bestmove(bestScore);
    return NodeControl::Continue;
}
