#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "typedefs.hpp"
#include "Square.hpp"
#include "PieceSquareTable.hpp"

class Evaluation {
    static const PieceSquareTable pieceSquareTable;

public:
    typedef index_t _t;

private:
    _t pst;
    EvalStage stage;

public:
    Evaluation () : pst{0}, stage(Middlegame) {}
    Evaluation (const Evaluation& e) = default;
    explicit operator _t () const { return pst; }
    void clear() { *this = {}; }

    void setStage(EvalStage evalStage, Square kingFrom) {
        if (stage == evalStage) {
            return;
        }

        clear((stage == Endgame ? KingEndgame : King), kingFrom);
        stage = evalStage;
        drop((stage == Endgame ? KingEndgame : King), kingFrom);
    }

    void drop(PieceType::_t ty, Square to) { pst += pieceSquareTable(ty, to); }
    void clear(PieceType::_t ty, Square from) { pst -= pieceSquareTable(ty, from); }

    void move(PieceType::_t ty, Square from, Square to) {
        assert (from != to);

        if (ty == King) {
            ty = (stage == Endgame ? KingEndgame : King);
        }

        clear(ty, from);
        drop(ty, to);
    }

    void promote(Square from, Square to, PromoType::_t ty) {
        assert (from.is(Rank7) && to.is(Rank8));
        clear(Pawn, from);
        drop(ty, to);
    }

};

#endif
