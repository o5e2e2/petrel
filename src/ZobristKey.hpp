#ifndef ZOBRIST_KEY_HPP
#define ZOBRIST_KEY_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class ZobristKey {
public:
    typedef u64_t _t;
    typedef PieceZobristType Index;

private:
    static constexpr _t rol(_t n, Square::_t sq) { return n << sq | n >> (64-sq); }

    //selected de Bruijn sequences
    enum : _t {
        zQueen  = 0x0218a392cd5d3dbfull,
        zRook   = 0x024530decb9f8eadull,
        zBishop = 0x02b91efc4b53a1b3ull,
        zKnight = 0x02dc61d5ecfc9a51ull,
        zPawn   = 0x031faf09dcda2ca9ull,
        zKing   = 0x0352138afdd1e65bull,
        zExtra  = 0x03ac4dfb48546797ull,
        zCastling = zExtra ^ zRook,
        zEnPassant = zExtra ^ zPawn,
    };

    const Index::arrayOf<_t> key = {{
        zQueen, zRook, zBishop, zKnight, zPawn, zKing, zCastling, zEnPassant
    }};

public:
    constexpr _t operator() (Index i, Square sq) const { return rol(key[i], sq); }

};

extern const ZobristKey zobristKey;

#endif
