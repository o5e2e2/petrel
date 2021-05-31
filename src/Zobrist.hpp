#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include "bitops.hpp"
#include "ZobristKey.hpp"

class Zobrist {
    typedef ZobristKey::Index Index;

public:
    typedef ZobristKey::_t _t;
    typedef Zobrist Arg;

private:
    _t _v;
    void change(Index ty, Square to) { _v ^= ::zobristKey(ty, to); }

public:
    constexpr explicit Zobrist (_t z) : _v{z} {}
    constexpr operator const _t& () const { return _v; }

    Zobrist (Arg my, Arg op) : _v{ my._v ^ ::bswap(op._v) } {}

    void drop(PieceType ty, Square to) { change(Index{ty}, to); }
    void clear(PieceType ty, Square from) { drop(ty, from); }

    void setCastling(Square sq)  { assert (sq.on(Rank1)); change(ZobristCastling, sq); }
    void clearCastling(Square sq) { setCastling(sq); }

    void setEnPassant(Square sq) { assert (sq.on(Rank4)); change(ZobristEnPassant, sq); }
    void setEnPassant(File fileFrom) { setEnPassant(Square{fileFrom, Rank4}); }
    void clearEnPassant(Square sq) { setEnPassant(sq); }

    void move(PieceType ty, Square from, Square to) {
        assert (from != to);
        clear(ty, from);
        drop(ty, to);
    }

    void promote(Square from, Square to, PromoType::_t ty) {
        assert (from.on(Rank7) && to.on(Rank8));
        clear(Pawn, from);
        drop(ty, to);
    }

    constexpr friend bool operator == (Arg a, Arg b) { return a._v == b._v; }

    friend io::ostream& operator << (io::ostream& out, Zobrist z) {
        auto flags(out.flags());

        out << std::hex << std::setw(16) << std::setfill('0');
        out << static_cast<Zobrist::_t>(z);

        out.flags(flags);
        return out;
    }

};

#endif
