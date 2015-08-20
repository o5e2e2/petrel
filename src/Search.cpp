#include "Search.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool perft(const Position& parent, SearchWindow& window) {
        Zobrist zobrist = parent.getZobrist();
        auto origin = window.control.tt().lookup(zobrist);

        PositionMoves pm(parent);
        pm.generateMoves();
        MatrixPiBb& moves = pm.getMoves();

        if (window.draft <= 0) {
            window.control.addPerftNodes(moves.count());
            return false;
        }

        {
            PerftTT tt(origin);
            auto n = tt.get(zobrist, window.draft);

            if (n) {
                window.control.addPerftNodes(n);
                return false;
            }
        }

        CUT ( window.control.checkQuota() );

        SearchWindow childWindow(window);

        auto n = window.control.getPerftNodes();
        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.decrementQuota();
                Position childPos(parent, from, to);
                CUT (perft(childPos, childWindow));
            }
        }

        PerftTT tt(origin);
        tt.set(zobrist, window.draft, window.control.getPerftNodes() - n);
        return false;
    }
}

namespace PerftDivide {
    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        pm.generateMoves();
        MatrixPiBb& moves = pm.getMoves();

        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                Position childPos{parent, from, to};

                window.control.decrementQuota();
                CUT (Perft::perft(childPos, childWindow));

                Move move = parent.createMove(My, from, to);
                window.control.report_perft_divide(move);
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(const Position& parent, SearchWindow& window) {
        PerftRecord::nextAge();
        bool isAborted = window.searchFn(parent, window);

        if (!isAborted) {
            window.control.report_perft_depth(window.draft);
        }

        return isAborted;
    }

    bool perft(const Position& parent, SearchWindow& window) {
        if (window.draft > 0) {
            perftX(parent, window);
        }
        else {
            for (window.draft = 1; !perftX(parent, window); ++window.draft) {
            }
        }

        window.control.report_bestmove();
        return true;
    }
}
