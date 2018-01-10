#include "NodeRoot.hpp"
#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "Move.hpp"

bool NodeRoot::searchIteration() {
    MatrixPiBb _moves = cloneMoves();

    for (Pi pi : alivePieces()) {
        Square from = squareOf(pi);

        for (Square to : _moves[pi]) {
            _moves.clear(pi, to);

            switch (draft) {
                case 0:
                    control.info.inc(PerftNodes, 1);
                    break;

                case 1:
                    CUT ( NodePerftLeaf(*this).visit(from, to) );
                    break;

                default:
                    CUT ( NodePerft(*this).visit(from, to) );
            }

            if (isDivide) {
                Move move = getSide(My).createMove(from, to);
                control.info.report_perft_divide(move);
            }
        }
    }

    control.info.report_perft_depth(draft);
    return false;
}

bool NodeRoot::iterativeDeepening() {
    for (draft = 1; draft <= DEPTH_MAX; ++draft) {
        CUT (searchIteration());
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
