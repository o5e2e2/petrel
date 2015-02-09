#include "Search.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool perftX(const Position& pos, const SearchControl& control, SearchWindow& window) {
        control.info.nodesQuota--;
        return perft(pos, control, window);
    }

    bool perft(const Position& parent, const SearchControl& control, SearchWindow& window) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        if (window.draft <= 0) {
            control.info.perftNodes += moves.count();
            return false;
        }

        CUT ( control.checkQuota() );

        SearchWindow childWindow(window);

        for (Pi pi : pm.side(My).alive()) {
            Square from{ pm.side(My).squareOf(pi) };

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                Position childPos(parent, from, to);
                CUT (perftX(childPos, control, childWindow));
            }
        }

        return false;
    }
}

namespace PerftDivide {
    bool perftX(const Position& pos, const SearchControl& control, SearchWindow& window) {
        control.info.nodesQuota--;
        CUT (Perft::perft(pos, control, window));
        control.info.report_perft_divide();
        return false;
    }

    bool perft(const Position& parent, const SearchControl& control, SearchWindow& window) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : pm.side(My).alive()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                control.info.currmove = parent.createMove(My, from, to);
                Position childPos{parent, from, to};

                CUT (perftX(childPos, control, childWindow));
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(const Position& parent, const SearchControl& control, SearchWindow& window) {
        bool isAborted = window.searchFn(parent, control, window);

        if (!isAborted) {
            control.info.depth = window.draft;
            control.info.report_perft_depth();
        }

        return isAborted;
    }

    bool perft(const Position& parent, const SearchControl& control, SearchWindow& window) {
        if (window.draft > 0) {
            perftX(parent, control, window);
        }
        else {
            for (window.draft = 1; !perftX(parent, control, window); ++window.draft) {}
        }

        control.info.report_bestmove();
        return true;
    }
}
