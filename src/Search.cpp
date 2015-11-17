#include "Search.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool _perft(Position& child, SearchWindow& window, const Position& parent, Square from, Square to) {
        if (window.draft <= 0) {
            child.makeMove({}, parent, from, to);

            PositionMoves pm(child);
            pm.generateMoves();
            MatrixPiBb& moves = pm.getMoves();

            window.control.info[PerftNodes] += moves.count();
            return false;
        }

        Zobrist zobrist = parent.makeZobrist(from, to);
        auto origin = window.control.tt().lookup(zobrist);

        CUT ( window.control.checkQuota() );

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

        child.makeMove(zobrist, parent, from, to);
        if (zobrist != child.getZobrist()) {
            //std::cout << from << to << ' ' << std::flush;
        }

        auto n = window.control.info[PerftNodes];
        CUT (perft(child, window));
        n = window.control.info[PerftNodes] - n;

        if (n) {
            PerftTT tt(origin, window.control.tt().getAge());
            tt.set(zobrist, window.draft, n);
        }

        return false;
    }

    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        pm.generateMoves();
        MatrixPiBb& moves = pm.getMoves();

        Position childPos;
        SearchWindow childWindow(window);

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.info.decrementQuota();
                CUT (_perft(childPos, childWindow, parent, from, to));
            }
        }

        return false;
    }
}

namespace PerftDivide {
    bool perft(const Position& parent, SearchWindow& window) {
        PositionMoves pm(parent);
        pm.generateMoves();
        MatrixPiBb& moves = pm.getMoves();

        Position childPos;
        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : pm.side(My).alivePieces()) {
            Square from = pm.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                window.control.info.decrementQuota();
                CUT (Perft::_perft(childPos, childWindow, parent, from, to));

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
