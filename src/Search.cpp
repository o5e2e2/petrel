#include "Search.hpp"
#include "PerftTT.hpp"
#include "PerftRecord.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "SearchWindow.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

namespace Perft {
    bool _perft(PositionMoves& parent, SearchWindow& window);

    bool makeMove(PositionMoves& child, SearchWindow& window, Square from, Square to) {
        auto& info = window.control.info;

        CUT ( window.control.checkQuota() );
        info.decrementQuota();

        if (window.draft <= 0) {
            child.makeMove(from, to);
            auto n = child.getMoves().count();
            info.inc(PerftNodes, n);
            return false;
        }

        Zobrist zobrist = child.makeZobrist(from, to);
        auto origin = window.control.tt().lookup(zobrist);

        {
            auto n = PerftTT(origin, window.control.tt().getAge()).get(zobrist, window.draft);
            info.inc(TT_Tried);

            if (n != NODE_COUNT_NONE) {
                info.inc(TT_Hit);
                info.inc(PerftNodes, n);
                return false;
            }
        }

        child.makeMove(from, to, zobrist);
        assert (zobrist == child.getPos().generateZobrist());

        auto n = info.get(PerftNodes);
        CUT (_perft(child, window));
        n = info.get(PerftNodes) - n;

        PerftTT(origin, window.control.tt().getAge()).set(zobrist, window.draft, n);
        info.inc(TT_Written);

        return false;
    }

    template <typename InfoMove>
    bool _perft(const PositionMoves& parent, SearchWindow& window, MatrixPiBb& moves, InfoMove infoMove) {
        PositionMoves childPosMoves(parent, 0);
        SearchWindow childWindow(window);

        for (Pi pi : parent.side(My).alivePieces()) {
            Square from = parent.side(My).squareOf(pi);

            for (Square to : moves[pi]) {
                moves.clear(pi, to);

                CUT (makeMove(childPosMoves, childWindow, from, to));
                infoMove(parent, window, from, to);
            }
        }

        return false;
    }

    bool _perft(PositionMoves& parent, SearchWindow& window) {
        return _perft(parent, window, parent.getMoves(), [](const PositionMoves&, SearchWindow&, Square, Square) {});
    }

    bool perft(const PositionMoves& parent, SearchWindow& window) {
        MatrixPiBb moves = parent.cloneMoves();

        CUT (_perft(parent, window, moves, [](const PositionMoves&, SearchWindow&, Square, Square) {}));

        window.control.info.report_perft_depth(window.draft);
        return false;
    }

    bool divide(const PositionMoves& parent, SearchWindow& window) {
        MatrixPiBb moves = parent.cloneMoves();

        auto divideMove = [](const PositionMoves& _parent, SearchWindow& _window, Square from, Square to) {
            Move move = _parent.getPos().createMove(from, to);
            _window.control.info.report_perft_divide(move);
        };

        CUT (_perft(parent, window, moves, divideMove));

        window.control.info.report_perft_depth(window.draft);
        return false;
    }

    bool perftRoot(const PositionMoves& parent, SearchWindow& window) {
        window.searchFn(parent, window);

        window.control.info.bestmove();
        return false;
    }

    bool perftId(const PositionMoves& parent, SearchWindow& window) {
        for (window.draft = 1; window.draft < SearchLimit::MaxDepth; ++window.draft) {
            if (window.searchFn(parent, window)) {
                break;
            }
            window.control.nextIteration();
        }

        window.control.info.bestmove();
        return false;
    }

}
