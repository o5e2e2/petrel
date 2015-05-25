#include "ZobristKey.hpp"

namespace {
    constexpr ZobristKey::_t rol(ZobristKey::_t n, Square::_t sq) { return n << sq | n >> (64-sq); }
}

ZobristKey::ZobristKey () {
    _t init[PieceTag::Size] = {
        0x0218a392cd5d3dbfull,
        0x024530decb9f8eadull,
        0x02b91efc4b53a1b3ull,
        0x02dc61d5ecfc9a51ull,
        0x031faf09dcda2ca9ull,
        0x0352138afdd1e65bull,
        0x03ac4dfb48546797ull,
    };

    FOR_INDEX(PieceTag, ty) {
        FOR_INDEX(Square, sq) {
            key[ty][sq] = rol(init[ty], sq);
        }
    }

    FOR_INDEX(File, file) {
        key[SpecialPiece][Square(file, Rank4)] ^= key[Pawn][Square(file, Rank4)]; //en passant
        key[SpecialPiece][Square(file, Rank1)] ^= key[Rook][Square(file, Rank1)]; //castling
    }
}
