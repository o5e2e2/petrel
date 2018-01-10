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

            if (draft == 0) {
                control.info.inc(PerftNodes, 1);
            }
            else if (draft == 1) {
                CUT ( NodePerftLeaf(*this).visit(from, to) );
            }
            else {
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

bool NodeRoot::visitChildren() {
    if (draft > 0) {
        searchIteration();
    }
    else {
        for (draft = 1; draft <= DEPTH_MAX; ++draft) {
            if (searchIteration()) {
                break;
            }
            control.nextIteration();
        }
    }
    control.info.bestmove();
    return false;
}
