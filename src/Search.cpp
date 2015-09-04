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
            window.control.info[PerftNodes] += moves.count();
            return false;
        }

        {
            PerftTT tt(origin);
            auto n = tt.get(zobrist, window.draft, window.control.info);

            if (n) {
                window.control.info[PerftNodes] += n;
                return false;
            }
        }

        CUT ( window.control.checkQuota() );

        SearchWindow childWindow(window);

        auto n = window.control.info[PerftNodes];
        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.info.decrementQuota();
                Position childPos(parent, from, to);
                CUT (perft(childPos, childWindow));
            }
        }

        PerftTT tt(origin);
        tt.set(zobrist, window.draft, window.control.info[PerftNodes] - n, window.control.info);
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

                window.control.info.decrementQuota();
                CUT (Perft::perft(childPos, childWindow));

                Move move{parent, from, to};
                window.control.info.report_perft_divide(move);
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(const Position& parent, SearchWindow& window) {
        PerftRecord::nextAge();
        window.control.info.clearNodes();
        bool isAborted = window.searchFn(parent, window);

        if (!isAborted) {
            window.control.info.report_perft_depth(window.draft);
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

        window.control.info.report_bestmove();
        return true;
    }
}
