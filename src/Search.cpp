#include "Search.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool perftX(const Position& pos, SearchWindow& window) {
        window.control.info.nodesQuota--;
        return perft(pos, window);
    }

    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        if (window.draft <= 0) {
            window.control.info.perftNodes += moves.count();
            return false;
        }

        CUT ( window.control.checkQuota() );

        SearchWindow childWindow(window);

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from{ pm.side(My).squareOf(pi) };

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                Position childPos(parent, from, to);
                CUT (perftX(childPos, childWindow));
            }
        }

        return false;
    }
}

namespace PerftDivide {
    bool perftX(const Position& pos, SearchWindow& window) {
        window.control.info.nodesQuota--;
        CUT (Perft::perft(pos, window));
        window.control.info.report_perft_divide();
        return false;
    }

    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                childWindow.control.info.currmove = parent.createMove(My, from, to);
                Position childPos{parent, from, to};

                CUT (perftX(childPos, childWindow));
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(const Position& parent, SearchWindow& window) {
        bool isAborted = window.searchFn(parent, window);

        if (!isAborted) {
            window.control.info.depth = window.draft;
            window.control.info.report_perft_depth();
        }

        return isAborted;
    }

    bool perft(const Position& parent, SearchWindow& window) {
        if (window.draft > 0) {
            perftX(parent, window);
        }
        else {
            for (window.draft = 1; !perftX(parent, window); ++window.draft) {}
        }

        window.control.info.report_bestmove();
        return true;
    }
}
