#include "PieceTypeAttack.hpp"

constexpr signed Square::x88(signed d_file, signed d_rank) const {
    return this->_v + (this->_v & 070) + d_file + 16*d_rank;
}

constexpr Bb Square::operator() (signed d_file, signed d_rank) const {
    return (x88(d_file, d_rank) & 0x88)? Bb::empty() : Bb(static_cast<_t>((x88(d_file, d_rank) + (x88(d_file, d_rank) & 7)) >> 1));
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
