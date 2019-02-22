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
    GamePhase gamePhase;

    PieceEvalType kingType() { return (gamePhase == Endgame) ? KingEndgame : King; }

public:
    constexpr Evaluation () : pst{0}, gamePhase{Middlegame} {}
    Evaluation (const Evaluation&) = default;

    Score evaluate() const { return static_cast<Score>(5 * static_cast<Score>(pst)); }

    void drop(PieceType::_t ty, Square to) { pst += pieceSquareTable(ty, to); }
    void clear(PieceType::_t ty, Square from) { pst -= pieceSquareTable(ty, from); }

    void setGamePhase(GamePhase evalGamePhase, Square kingFrom) {
        if (gamePhase == evalGamePhase) {
            return;
        }

        clear(kingType(), kingFrom);
        gamePhase = evalGamePhase;
        drop(kingType(), kingFrom);
    }

    void move(PieceType::_t ty, Square from, Square to) {
        assert (from != to);

        if (ty == King) {
            ty = kingType();
        }

        clear(ty, from);
        drop(ty, to);
    }

    void castle(Square kingFrom, Square kingTo, Square rookFrom, Square rookTo) {
        auto theKing = kingType();

        clear(theKing, kingFrom);
        clear(Rook, rookFrom);
        drop(theKing, kingTo);
        drop(Rook, rookTo);
    }

    void promote(Square from, Square to, PromoType::_t ty) {
        assert (from.on(Rank7) && to.on(Rank8));
        clear(Pawn, from);
        drop(ty, to);
    }

};

#endif
