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

bool NodePerftRoot::searchIteration() {
    if (isDivide) {
        CUT ( NodePerftRootDivide(*this).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                perft += getMovesCount();
                break;

            case 2:
                CUT ( NodePerftLeaf(*this).visitChildren() );
                break;

            default:
                assert (draft >= 3);
                CUT ( NodePerftTT(*this, draft-1).visitChildren() );
        }
    }

    control.perftDepth(draft, perft, bestMove, bestScore);
    return false;
}

bool NodePerftRoot::iterativeDeepening() {
    for (draft = 1; draft <= DepthMax; ++draft) {
        CUT ( searchIteration() );

        perft = 0;
        bestMove = {};
        bestScore = Score::None;

        control.nextIteration();
    }

    return false;
}

bool NodePerftRoot::visitChildren() {
    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.bestmove(bestMove, bestScore);
    return false;
}
