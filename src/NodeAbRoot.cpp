#include "NodeAbRoot.hpp"
#include "NodeAbPv.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"

NodeAbRoot::NodeAbRoot (const SearchLimit& limit, SearchControl& searchControl):
    NodeAbPv(limit.positionMoves, searchControl, 1), depthLimit(limit.depth ? limit.depth : DepthMax)
{}

NodeControl NodeAbRoot::searchIteration() {
    RETURN_IF_ABORT ( NodeAbPv(*this).visitChildren() );
    control.infoDepth(draft, bestScore);
    return NodeControl::Continue;
}

NodeControl NodeAbRoot::iterativeDeepening() {
    for (draft = 1; draft <= depthLimit; ++draft) {
        RETURN_IF_ABORT ( searchIteration() );
        bestScore = Score::None;
        control.newIteration();
    }

    return NodeControl::Continue;
}

NodeControl NodeAbRoot::visitChildren() {
    iterativeDeepening();

    control.bestmove(bestScore);
    return NodeControl::Abort;
}
