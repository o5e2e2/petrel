#ifndef ZOBRIST_KEY_HPP
#define ZOBRIST_KEY_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class CACHE_ALIGN ZobristKey {
public:
    typedef std::uint64_t _t;

    enum { Castling = 6, EnPassant = 7 };
    typedef ::Index<8, piece_type_t> Index;

private:
    static constexpr _t rol(_t n, Square::_t sq) { return n << sq | n >> (64-sq); }

    enum : _t {
        Queen  = 0x0218a392cd5d3dbfull,
        Rook   = 0x024530decb9f8eadull,
        Bishop = 0x02b91efc4b53a1b3ull,
        Knight = 0x02dc61d5ecfc9a51ull,
        Pawn   = 0x031faf09dcda2ca9ull,
        King   = 0x0352138afdd1e65bull,
        Extra  = 0x03ac4dfb48546797ull
    };

    const Index::array<_t> key = {{
        Queen, Rook, Bishop, Knight, Pawn, King,
        Extra ^ Rook, //castling
        Extra ^ Pawn //en passant
    }};

public:
    constexpr _t operator() (Index i, Square sq) const { return rol(key[i], sq); }

};

#endif
