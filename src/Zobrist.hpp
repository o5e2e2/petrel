#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "Index.hpp"

class ZobristTable {
public:
    typedef U64 _t;
    _t z[PieceType::Size][Square::Size];

public:
    ZobristTable () {
        std::mt19937_64 random;
        FOR_INDEX(PieceType, ty) {
            FOR_INDEX(Square, sq) {
                z[ty][sq] = random();
            }
        }
    }

    const _t& operator() (PieceType ty, Square sq) const { return z[ty][sq]; }
};

class Zobrist {
    static const ZobristTable z;

public:
    typedef ZobristTable::_t _t;

private:
    _t _v;

public:
    Zobrist () : _v{0} {}
    Zobrist (const Zobrist& z) : _v(z) {}
    explicit operator _t () const { return _v; }
    void clear() { _v = 0; }

    Zobrist& flip() { _v = ::bswap(_v); return *this; }
    Zobrist operator ~ () const { return Zobrist{*this}.flip(); }

    void drop(PieceType ty, Square to) { _v ^= z(ty, to); }
    void clear(PieceType ty, Square from) { drop(ty, from); }

    void setEnPassant(Square from) { drop( Pawn, Square(File(from), Rank1) ); }
    void clearEnPassant(Square from) { setEnPassant(from); }

    void setCastling(Square from) { drop( Pawn, Square(File(from), Rank8) ); }
    void clearCastling(Square from) { setCastling(from); }

    Zobrist& operator+= (Zobrist other) { _v ^= other._v; return *this; }
    static Zobrist combine(Zobrist my, Zobrist op) { return Zobrist{my} += ~op; }
};

#endif
