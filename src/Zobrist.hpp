#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include "bitops.hpp"
#include "typedefs.hpp"
#include "Square.hpp"
#include "ZobristKey.hpp"

class Zobrist {
public:
    typedef ZobristKey::_t _t;
    typedef Zobrist Arg;

private:
    _t _v;

public:
    constexpr Zobrist () : _v{0} {}
    constexpr explicit Zobrist (_t z) : _v(z) {}
    Zobrist (Arg my, Arg op) : _v{ my._v ^ ~::bswap(op._v) } {}
    constexpr operator const _t& () const { return _v; }

    void clear() { *this = {}; }

    Zobrist& flip() { _v = ~::bswap(_v); return *this; }
    Zobrist operator ~ () const { return Zobrist{*this}.flip(); }

    void drop(PieceType ty, Square to) { _v ^= zobristKey(ty, to); }
    void clear(PieceType ty, Square from) { drop(ty, from); }

    void setEnPassant(File fileFrom) { drop( Pawn, Square(fileFrom, Rank8) ); }
    void clearEnPassant(File fileFrom) { setEnPassant(fileFrom); }

    void setCastling(Square from) { assert (from.is(Rank1)); drop(Pawn, from); }
    void clearCastling(Square from) { setCastling(from); }

    void move(PieceType ty, Square from, Square to) { clear(ty, from); drop(ty, to); }

    void promote(Square from, Square to, PromoType ty) {
        assert (from.is(Rank7) && to.is(Rank8));
        clear(Pawn, from);
        drop(static_cast<PieceType::_t>(ty), to);
    }

    constexpr friend bool operator == (Arg a, Arg b) { return a._v == b._v; }
};

#endif
