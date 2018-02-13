#include "NodePerftRoot.hpp"
#include "NodePerftTT.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "Move.hpp"
#include "SearchControl.hpp"

bool NodePerftRoot::searchIteration() {
    if (control.searchLimit.isDivide()) {
        CUT ( NodePerftRootDivide(*this).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                perft += getMoves().count();
                break;

            case 2:
                CUT ( NodePerftLeaf(*this).visitChildren() );
                break;

            default:
                CUT ( NodePerftTT(*this, draft-1).visitChildren() );
        }
    }

    control.info.report_perft_depth(draft, perft);
    perft = 0;
    return false;
}

bool NodePerftRoot::iterativeDeepening() {
    MatrixPiBb _moves = cloneMoves();

    for (draft = 1; draft <= DEPTH_MAX; ++draft) {
        CUT (searchIteration());

        this->moves = _moves;
        control.nextIteration();
    }

    return false;
}

bool NodePerftRoot::visitChildren() {
    draft = control.searchLimit.getDepth();

    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.info.bestmove();
    return false;
}
