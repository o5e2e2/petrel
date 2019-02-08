#include "NodePerftRoot.hpp"
#include "NodePerftTT.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciSearchInfo.hpp"

NodePerftRoot::NodePerftRoot (const SearchLimit& l, SearchControl& c):
    NodePerft(l.getMovesLimit(), c, l.getDepthLimit()),
    isDivide(l.isDivide())
{}

bool NodePerftRoot::searchIteration() {
    if (isDivide) {
        CUT ( NodePerftRootDivide(*this).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                perft += movesCount;
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
    MatrixPiBb movesBackup = cloneMoves();

    for (draft = 1; draft <= DepthMax; ++draft) {
        CUT ( searchIteration() );

        moves = movesBackup;
        perft = 0;
        bestMove = {};
        bestScore = Score::Minimum;

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
