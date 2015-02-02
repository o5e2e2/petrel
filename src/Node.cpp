#include "Node.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool perftX(SearchControl& control, const Position& pos, depth_t draft) {
        control.info.nodesRemaining--;
        return perft(control, pos, draft);
    }

    bool perft(SearchControl& control, const Position& parent, depth_t draft) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        if (draft <= 0) {
            control.info.perftNodes += moves.count();
            return false;
        }

        CUT ( control.checkQuota() );

        for (Pi pi : pm.side(My).alive()) {
            Square from{ pm.side(My).squareOf(pi) };

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                Position pos(parent, from, to);

                CUT (perftX(control, pos, draft-1));
            }
        }

        return false;
    }
}

namespace PerftDivide {
    bool perftX(SearchControl& control, const Position& pos, depth_t draft) {
        auto perftTotal = control.info.perftNodes;
        control.info.perftNodes = 0;

        control.info.nodesRemaining--;
        CUT (Perft::perft(control, pos, draft));

        control.info.currmovenumber++;
        control.report_perft_divide();

        control.info.perftNodes += perftTotal;
        return false;
    }

    bool perft(SearchControl& control, const Position& parent, depth_t draft) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        for (Pi pi : pm.side(My).alive()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                control.info.currmove = parent.createMove(My, from, to);
                Position pos{parent, from, to};

                CUT (perftX(control, pos, draft-1));
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(SearchControl& control, const Position& parent, depth_t depth) {
        control.info.perftNodes = 0;
        control.info.depth = depth;

        bool isAborted = Perft::perft(control, parent, depth);

        if (!isAborted) {
            control.report_perft_depth();
        }

        return isAborted;
    }

    bool perft(SearchControl& control, const Position& parent, depth_t depth) {
        if (depth > 0) {
            perftX(control, parent, depth);
        }
        else {
            for (depth = 1; !perftX(control, parent, depth); ++depth) {}
        }

        control.report_bestmove();
        return true;
    }
}

namespace PerftDivideRoot {
    bool perftX(SearchControl& control, const Position& parent, depth_t depth) {
        control.info.perftNodes = 0;
        control.info.currmovenumber = 0;
        control.info.depth = depth;

        bool isAborted = PerftDivide::perft(control, parent, depth);

        if (!isAborted) {
            control.report_perft_depth();
        }

        return isAborted;
    }

    bool perft(SearchControl& control, const Position& parent, depth_t depth) {
        if (depth > 0) {
            perftX(control, parent, depth);
        }
        else {
            for (depth = 1; !perftX(control, parent, depth); ++depth) {}
        }

        control.report_bestmove();
        return true;
    }
}
