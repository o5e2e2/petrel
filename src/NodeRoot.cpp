#include "NodeRoot.hpp"
#include "NodePerft.hpp"
#include "NodePerftLeaf.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "Move.hpp"

bool NodeRoot::searchDepth() {
    MatrixPiBb _moves = cloneMoves();

    for (Pi pi : alivePieces()) {
        Square from = squareOf(pi);

        for (Square to : _moves[pi]) {
            _moves.clear(pi, to);

            if (draft == 0) {
                control.info.inc(PerftNodes, 1);
            }
            else if (draft == 1) {
                NodePerftLeaf child(*this);
                CUT (child.visit(from, to));
            }
            else {
                NodePerft child(*this);
                CUT (child.visit(from, to));
            }

            if (isDivide) {
                Move move = Move::createMove(getSide(My), from, to);
                control.info.report_perft_divide(move);
            }
        }
    }

    control.info.report_perft_depth(draft);
    return false;
}

bool NodeRoot::visitChildren(Node&) {
    if (draft > 0) {
        searchDepth();
    }
    else {
        for (this->draft = 1; this->draft < SearchLimit::MaxDepth; ++this->draft) {
            if (searchDepth()) {
                break;
            }
            control.nextIteration();
        }
    }
    control.info.bestmove();
    return false;
}
