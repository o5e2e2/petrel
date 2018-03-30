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

public:
    constexpr Evaluation () : pst{0}, gamePhase{Middlegame} {}
    Evaluation (const Evaluation&) = default;

    Score evaluate() const { return static_cast<Score>(5 * static_cast<Score>(pst)); }

    void setGamePhase(GamePhase evalGamePhase, Square kingFrom) {
        if (gamePhase == evalGamePhase) {
            return;
        }

        clear((gamePhase == Endgame ? KingEndgame : King), kingFrom);
        gamePhase = evalGamePhase;
        drop((gamePhase == Endgame ? KingEndgame : King), kingFrom);
    }

    void drop(PieceType::_t ty, Square to) { pst += pieceSquareTable(ty, to); }
    void clear(PieceType::_t ty, Square from) { pst -= pieceSquareTable(ty, from); }

    void move(PieceType::_t ty, Square from, Square to) {
        assert (from != to);

        if (ty == King) {
            ty = (gamePhase == Endgame ? KingEndgame : King);
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
