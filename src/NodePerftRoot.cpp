#include "NodePerftRoot.hpp"
#include "NodePerftTT.hpp"
#include "NodePerftLeaf.hpp"
#include "NodePerftRootDivide.hpp"
#include "Move.hpp"

bool NodePerftRoot::searchIteration(depth_t draft) {
    if (control.searchLimit.isDivide()) {
        CUT ( NodePerftRootDivide(*this, control, draft).visitChildren() );
    }
    else {
        switch (draft) {
            case 1:
                control.info.inc(PerftNodes, getMoves().count());
                break;

            case 2:
                CUT ( NodePerftLeaf(*this).visitChildren() );
                break;

            default:
                CUT ( NodePerftTT(*this, draft-1).visitChildren() );
        }
    }

    control.info.report_perft_depth(draft);
    return false;
}

bool NodePerftRoot::iterativeDeepening() {
    MatrixPiBb _moves = cloneMoves();

    for (depth_t draft = 1; draft <= DEPTH_MAX; ++draft) {
        CUT (searchIteration(draft));

        this->moves = _moves;
        control.nextIteration();
    }

    return false;
}

bool NodePerftRoot::visitChildren() {
    auto draft = control.searchLimit.getDepth();

    if (draft > 0) {
        searchIteration(draft);
    }
    else {
        iterativeDeepening();
    }

    control.info.bestmove();
    return false;
}
