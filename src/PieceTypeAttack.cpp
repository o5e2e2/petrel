#include "PieceTypeAttack.hpp"

constexpr Bb Square::operator() (signed fileOffset, signed rankOffset) const {
    return (x88(fileOffset, rankOffset) & 0x88)? Bb::empty() : Bb(static_cast<_t>((x88(fileOffset, rankOffset) + (x88(fileOffset, rankOffset) & 7)) >> 1));
}

PieceTypeAttack::PieceTypeAttack () {
    FOR_INDEX(Square, sq) {
        attack[Rook][sq] = Bb::vertical(sq) + Bb::horizont(sq);
        attack[Bishop][sq] = Bb::diagonal(sq) + Bb::antidiag(sq);
        attack[Queen][sq] = attack[Rook][sq] + attack[Bishop][sq];

        attack[Pawn][sq] = sq(-1, -1) + sq(+1, -1);

        attack[Knight][sq] =
               sq(-2, -1) + sq(-2, +1) + sq(-1, -2) + sq(-1, +2)
             + sq(+2, +1) + sq(+2, -1) + sq(+1, +2) + sq(+1, -2);

        attack[King][sq] =
               sq(-1, -1) + sq(-1, 0) + sq(-1, +1) + sq(0, -1)
             + sq(+1, +1) + sq(+1, 0) + sq(+1, -1) + sq(0, +1);
    }
}
