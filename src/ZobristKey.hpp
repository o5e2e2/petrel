#ifndef ZOBRIST_KEY_HPP
#define ZOBRIST_KEY_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class ZobristKey {
public:
    typedef std::uint64_t _t;

private:
    _t key[PieceTag::Size][Square::Size];

    constexpr static _t r(_t n, Square::_t sq) { return n << sq | n >> (64-sq); }

public:
    ZobristKey () {
        _t init[PieceTag::Size] = {
            0x0218a392d367abbfull,
            0x0218fd49de59b457ull,
            0x021b2a4fd16bc773ull,
            0x0323dba73562fc25ull,
            0x032fc73dbac2a4d1ull,
            0x03422eadec73253full,
            0x026763d5c37e5a45ull,
        };

        FOR_INDEX(PieceTag, ty) {
            FOR_INDEX(Square, sq) {
                key[ty][sq] = r(init[ty], sq);
            }
        }

        FOR_INDEX(File, file) {
            key[SpecialPiece][Square(file, Rank4)] ^= key[Pawn][Square(file, Rank4)]; //en passant
            key[SpecialPiece][Square(file, Rank1)] ^= key[Rook][Square(file, Rank1)]; //castling
        }
    }

    const _t& operator() (PieceTag ty, Square sq) const { return key[ty][sq]; }

};

extern const ZobristKey zobristKey;

#endif
