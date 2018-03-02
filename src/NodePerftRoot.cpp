#include "NodePerftRoot.hpp"
#include "NodePerftTT.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciSearchInfo.hpp"

NodePerftRoot::NodePerftRoot (const SearchLimit& l, SearchControl& c):
    NodePerft(l.getPositionMoves(), c, l.getDepth()),
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
                CUT ( NodePerftTT(*this, draft-1).visitChildren() );
        }
    }

    static_cast<UciSearchInfo&>(control.info).report_perft_depth(draft, perft);
    perft = 0;
    return false;
}

bool NodePerftRoot::iterativeDeepening() {
    MatrixPiBb movesBackup = cloneMoves();

    for (draft = 1; draft <= DEPTH_MAX; ++draft) {
        CUT ( searchIteration() );

        moves = movesBackup;
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

    control.info.bestmove();
    return false;
}
