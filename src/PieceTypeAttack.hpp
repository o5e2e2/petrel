#ifndef PIECE_TYPE_ATTACK_HPP
#define PIECE_TYPE_ATTACK_HPP

#include "Bb.hpp"

//attack bitboards of the piece types on the empty board (3.5k)
class PieceTypeAttack {
    PieceType::static_array< Square::static_array<Bb> > attack;

public:
    PieceTypeAttack ();

    const Bb& operator() (PieceType ty, Square sq) const {
        return attack[ty][sq];
    }
};

extern const PieceTypeAttack pieceTypeAttack;

#endif
