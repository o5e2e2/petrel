#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include "bitops.hpp"
#include "typedefs.hpp"
#include "Square.hpp"
#include "ZobristKey.hpp"

class Zobrist {
    static const ZobristKey zobristKey;
    typedef ZobristKey::Index Index;

public:
    typedef ZobristKey::_t _t;
    typedef Zobrist Arg;

private:
    _t _v;
    void drop(Index ty, Square to) { _v ^= zobristKey(ty, to); }
    void clear(Index ty, Square from) { drop(ty, from); }

public:
    constexpr Zobrist () : _v{0} {}
    constexpr explicit Zobrist (_t z) : _v{z} {}
    Zobrist (Arg my, Arg op) : _v{ my._v ^ ::bswap(op._v) } {}
    constexpr operator const _t& () const { return _v; }

    void clear() { *this = {}; }

    void drop(PieceType::_t ty, Square to) { drop(Index{ty}, to); }
    void clear(PieceType::_t ty, Square from) { drop(ty, from); }

    void setCastling(Square sq)  { assert (sq.is(Rank1)); drop(Castling, sq); }
    void setEnPassant(Square sq) { assert (sq.is(Rank4)); drop(EnPassant, sq); }
    void setEnPassant(File fileFrom) { setEnPassant(Square(fileFrom, Rank4)); }

    void clearCastling(Square sq) { setCastling(sq); }
    void clearEnPassant(Square sq) { setEnPassant(sq); }

    void move(PieceType ty, Square from, Square to) {
        assert (from != to);
        clear(ty, from);
        drop(ty, to);
    }

    void promote(Square from, Square to, PromoType::_t ty) {
        assert (from.is(Rank7) && to.is(Rank8));
        clear(Pawn, from);
        drop(ty, to);
    }

    constexpr friend bool operator == (Arg a, Arg b) { return a._v == b._v; }

    friend std::ostream& operator << (std::ostream& out, Zobrist z) {
        auto flags(out.flags());

        out << std::hex << std::setw(16) << std::setfill('0');
        out << static_cast<Zobrist::_t>(z);

        out.flags(flags);
        return out;
    }

};

#endif
