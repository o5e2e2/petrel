#include "NodeRoot.hpp"
#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "Move.hpp"

bool NodeRoot::visit(Square from, Square to) {
    switch (draft) {
        case 1:
            control.info.inc(PerftNodes, 1);
            break;

        case 2:
            CUT ( NodePerftLeaf(*this).visit(from, to) );
            break;

        default:
            CUT ( NodePerft(*this).visit(from, to) );
    }

    if (isDivide) {
        Move move = getSide(My).createMove(from, to);
        control.info.report_perft_divide(move);
    }

    return false;
}

bool NodeRoot::searchIteration() {
    CUT (Node::visitChildren());

    control.info.report_perft_depth(draft);
    return false;
}

bool NodeRoot::iterativeDeepening() {
    MatrixPiBb _moves = cloneMoves();

    for (draft = 1; draft <= DEPTH_MAX; ++draft) {
        CUT (searchIteration());

        this->moves = _moves;
        control.nextIteration();
    }

    return false;
}

bool NodeRoot::visitChildren() {
    if (draft > 0) {
        searchIteration();
    }
    else {
        iterativeDeepening();
    }

    control.info.bestmove();
    return false;
}
