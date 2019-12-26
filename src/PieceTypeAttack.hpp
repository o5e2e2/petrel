#ifndef PIECE_TYPE_ATTACK_HPP
#define PIECE_TYPE_ATTACK_HPP

#include "Bb.hpp"

//attack bitboards of the piece types on the empty board (3.5k)
class PieceTypeAttack {
    PieceType::static_array< Square::static_array<Bb> > attack;
public:
    constexpr PieceTypeAttack () {
        FOR_INDEX (Square, sq) {
            attack[Rook][sq] = sq.vertical() + sq.horizont();
            attack[Bishop][sq] = sq.diagonal() + sq.antidiag();

            attack[Queen][sq] =
                sq.vertical() + sq.horizont() +
                sq.diagonal() + sq.antidiag();

            attack[Pawn][sq] = sq(-1, -1) + sq(+1, -1);

            attack[Knight][sq] =
                sq(+2, +1) + sq(+2, -1) + sq(+1, +2) + sq(+1, -2) +
                sq(-2, -1) + sq(-2, +1) + sq(-1, -2) + sq(-1, +2);

            attack[King][sq] =
                sq(+1, +1) + sq(+1, 0) + sq(0, +1) + sq(+1, -1) +
                sq(-1, -1) + sq(-1, 0) + sq(0, -1) + sq(-1, +1);
        }
    }

    constexpr const Bb& operator() (PieceType ty, Square sq) const {
        return attack[ty][sq];
    }
};

extern const PieceTypeAttack attacksFrom;

#endif
