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
            PerftTT tt(origin, window.control.tt().getAge());
            auto n = tt.get(zobrist, window.draft);

            ++window.control.info[TT_Tried];
            if (n) {
                ++window.control.info[TT_Hit];
                window.control.info[PerftNodes] += n;
                return false;
            }
        }

        CUT ( window.control.checkQuota() );

        SearchWindow childWindow(window);
        Position childPos;

        auto n = window.control.info[PerftNodes];
        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.info.decrementQuota();
                Zobrist z = parent.makeZobrist(from, to);
                childPos.makeMove(z, parent, from, to);
                if (z != childPos.getZobrist()) {
                    std::cout << from << to << ' ' << std::flush;
                }
                CUT (perft(childPos, childWindow));
            }
        }

        n = window.control.info[PerftNodes] - n;
        if (n) {
            PerftTT tt(origin, window.control.tt().getAge());
            tt.set(zobrist, window.draft, n);
        }
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

        Position childPos;

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                childPos.makeMove(parent.makeZobrist(from, to), parent, from, to);

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
        window.control.nextIteration();

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
