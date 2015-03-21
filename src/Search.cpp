#include "Search.hpp"
#include "PerftTT.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool perft(const Position& parent, SearchWindow& window) {
        char* record( window.control.tt().lookup(parent.getZobrist()));

        PerftTT tt(record);
        node_count_t n = tt.get(parent.getZobrist(), window.draft);
        if (n && !parent.side[My].hasEnPassant()) {
            window.control.info.perftNodes += n;
            return false;
        }

        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        if (window.draft <= 0) {
            window.control.info.perftNodes += moves.count();
            return false;
        }

        CUT ( window.control.checkQuota() );

        SearchWindow childWindow(window);

        n = window.control.info.perftNodes;
        for (Pi pi : pm.side(My).alivePieces()) {
            Square from{ pm.side(My).squareOf(pi) };

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.info.nodesQuota--;
                Position childPos(parent, from, to);
                CUT (perft(childPos, childWindow));
            }
        }
        n = window.control.info.perftNodes - n;
        if (!parent.side[My].hasEnPassant()) {
            tt.set(parent.getZobrist(), window.draft, n);
        }
        return false;
    }
}

namespace PerftDivide {
    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        MatrixPiBb& moves = pm.getMoves();

        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                childWindow.control.info.currmove = parent.createMove(My, from, to);

                moves.clear(pi, to);

                Position childPos{parent, from, to};

                window.control.info.nodesQuota--;
                CUT (Perft::perft(childPos, childWindow));

                window.control.info.report_perft_divide();
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
