#include "NodePerftRoot.hpp"
#include "NodePerftTT.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"

NodePerftRoot::NodePerftRoot (const SearchLimit& limit, SearchControl& searchControl):
    NodePerft(limit.positionMoves, searchControl, limit.depth),
    isDivide(limit.isDivide)
{}

NodeControl NodePerftRoot::searchIteration() {
    if (isDivide) {
        RETURN_CONTROL ( NodePerftRootDivide(*this).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                perft += getMovesCount();
                break;

            case 2:
                RETURN_CONTROL ( NodePerftLeaf(*this).visitChildren() );
                break;

            default:
                assert (draft >= 3);
                RETURN_CONTROL ( NodePerft(*this).visitChildren() );
        }
    }

    control.infoPerftDepth(draft, perft, bestScore);
    return NodeControl::Continue;
}

NodeControl NodePerftRoot::iterativeDeepening() {
    for (draft = 1; draft <= DepthMax; ++draft) {
        RETURN_CONTROL ( searchIteration() );

        perft = 0;
        bestScore = Score::None;

        control.nextIteration();
    }

    return NodeControl::Continue;
}

NodeControl NodePerftRoot::visitChildren() {
    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.bestmove(bestScore);
    return NodeControl::Continue;
}
