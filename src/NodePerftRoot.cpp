#include "NodePerftRoot.hpp"
#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "Move.hpp"

bool NodePerftRoot::searchIteration() {
    if (isDivide) {
        CUT ( NodePerftRootDivide(*this, control, draft).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                control.info.inc(PerftNodes, getMoves().count());;
                break;

            case 2:
                CUT ( NodePerftLeaf(*this).visitChildren() );
                break;

            default:
                CUT ( NodePerft(*this).visitChildren() );
        }
    }

    control.info.report_perft_depth(draft);
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
    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.info.bestmove();
    return false;
}
