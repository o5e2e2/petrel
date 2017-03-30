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
    bool isEndgame;

public:
    Evaluation () : pst{0}, isEndgame{false} {}
    Evaluation (const Evaluation& e) : pst{e.pst}, isEndgame{e.isEndgame} {}
    explicit operator _t () const { return pst; }
    void clear() { *this = {}; }

    void setEndgame(bool endgame, Square kingFrom) {
        if (endgame == isEndgame) {
            return;
        }

        clear((isEndgame ? KingEndgame : King), kingFrom);
        isEndgame = endgame;
        drop((isEndgame ? KingEndgame : King), kingFrom);
    }

    void drop(PieceType::_t ty, Square to) { pst += pieceSquareTable(ty, to); }
    void clear(PieceType::_t ty, Square from) { pst -= pieceSquareTable(ty, from); }

    void move(PieceType::_t ty, Square from, Square to) {
        assert (from != to);

        if (ty == King) {
            ty = (isEndgame ? KingEndgame : King);
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
