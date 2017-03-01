#include "Search.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool makeMove(PositionMoves& child, SearchWindow& window, const Position& parent, Square from, Square to) {
        auto& info = window.control.info;

        info.decrementQuota();

        if (window.draft <= 0) {
            child.makeMove(parent, from, to);
            info[PerftNodes] += child.getMoves().count();
            return false;
        }

        Zobrist zobrist = parent.makeZobrist(from, to);
        auto origin = window.control.tt().lookup(zobrist);

        CUT ( window.control.checkQuota() );

        {
            ++info[TT_Tried];

            auto n = PerftTT(origin, window.control.tt().getAge()).get(zobrist, window.draft);
            if (n != NODE_COUNT_NONE) {
                ++info[TT_Hit];
                info[PerftNodes] += n;
                return false;
            }
        }

        child.makeMove(parent, from, to, zobrist);
        assert (zobrist == child.getPos().generateZobrist());

        auto n = info[PerftNodes];
        CUT (perft(child, window));

        ++info[TT_Written];

        n = info[PerftNodes] - n;
        PerftTT(origin, window.control.tt().getAge()).set(zobrist, window.draft, n);

        return false;
    }

    bool perft(PositionMoves& parent, SearchWindow& window) {
        MatrixPiBb& moves = parent.getMoves();

        PositionMoves childPosMoves;
        SearchWindow childWindow(window);

        for (Pi pi : parent.side(My).alivePieces()) {
            Square from = parent.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                CUT (makeMove(childPosMoves, childWindow, parent.getPos(), from, to));
            }
        }

        return false;
    }

    bool perft(const PositionMoves& parent, SearchWindow& window) {
        MatrixPiBb moves = parent.cloneMoves();

        PositionMoves childPosMoves;
        SearchWindow childWindow(window);

        for (Pi pi : parent.side(My).alivePieces()) {
            Square from = parent.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                CUT (makeMove(childPosMoves, childWindow, parent.getPos(), from, to));
            }
        }

        return false;
    }

}

namespace PerftDivide {
    bool perft(const PositionMoves& parent, SearchWindow& window) {
        MatrixPiBb moves = parent.cloneMoves();

        PositionMoves childPosMoves;
        SearchWindow childWindow(window);
        childWindow.searchFn = Perft::perft;

        for (Pi pi : parent.side(My).alivePieces()) {
            Square from = parent.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                CUT (Perft::makeMove(childPosMoves, childWindow, parent.getPos(), from, to));

                Move move = parent.getPos().createMove(from, to);
                window.control.info.report_perft_divide(move);
            }
        }

        return false;
    }
}

namespace PerftRoot {
    bool perftX(const PositionMoves& parent, SearchWindow& window) {
        window.control.nextIteration();

        bool isAborted = window.searchFn(parent, window);

        if (!isAborted) {
            window.control.info.report_perft_depth(window.draft);
        }

        return isAborted;
    }

    bool perft(const PositionMoves& parent, SearchWindow& window) {
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
