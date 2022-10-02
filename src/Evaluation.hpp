#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "typedefs.hpp"
#include "Score.hpp"
#include "Square.hpp"
#include "PieceSquareTable.hpp"

class Evaluation {
public:
    typedef PieceSquareTable::_t _t;
    typedef index_t game_phase_t;

    static constexpr PieceType::arrayOf<game_phase_t> tyGamePhase = {{ 1025, 477, 365, 337, 0, 0}};
    enum : game_phase_t { minEG = 518, maxMG = 6192, phaseRange = maxMG - minEG };

private:
    _t pst;
    game_phase_t gamePhase;

    constexpr void f(PieceType ty, Square from) { pst -= pieceSquareTable(ty, from); }
    constexpr void t(PieceType ty, Square to) { pst += pieceSquareTable(ty, to); }

    constexpr Score e(game_phase_t opGamePhase) const {
        const game_phase_t phase = std::max(static_cast<game_phase_t>(minEG), std::min(static_cast<game_phase_t>(maxMG), static_cast<game_phase_t>(this->gamePhase + opGamePhase)));
        const auto factorMG = (phase - minEG) / phaseRange;
        const auto factorEG = 1 - factorMG;

        auto mpst = pst & 0xffff;
        auto epst = pst >> 16;
        return static_cast<Score>(mpst * factorMG + epst * factorEG);
    }

public:
    constexpr Evaluation () : pst{0}, gamePhase{0} {}
    Evaluation (const Evaluation&) = default;

    static Score evaluate(const Evaluation& my, const Evaluation& op) {
        return my.e(op.gamePhase) - op.e(my.gamePhase);
    }

    void drop(PieceType ty, Square to) {
        t(ty, to);
        gamePhase += tyGamePhase[ty];
    }

    void capture(PieceType ty, Square from) {
        f(ty, from);
        gamePhase -= tyGamePhase[ty];
    }

    void move(PieceType ty, Square from, Square to) {
        assert (from != to);

        f(ty, from);
        t(ty, to);
    }

    void castle(Square kingFrom, Square kingTo, Square rookFrom, Square rookTo) {
        assert (kingFrom != rookFrom);
        assert (kingTo != rookTo);

        f(King, kingFrom);
        f(Rook, rookFrom);
        t(Rook, rookTo);
        t(King, kingTo);
    }

    void promote(Square from, Square to, PromoType ty) {
        assert (from.on(Rank7) && to.on(Rank8));

        f(Pawn, from);
        drop(static_cast<PieceType::_t>(ty), to);
    }

};

#endif
