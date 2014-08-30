#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "Index.hpp"

class Zobrist {
    class Key {
    public:
        typedef std::uint64_t _t;
        _t key[PieceType::Size][Square::Size];

    public:
        Key () {
            std::mt19937_64 random;
            FOR_INDEX(PieceType, ty) {
                FOR_INDEX(Square, sq) {
                    key[ty][sq] = random();
                }
            }
        }

        const _t& operator() (PieceType ty, Square sq) const { return key[ty][sq]; }
    };

    static const Key key;

public:
    typedef Key::_t _t;
    typedef Zobrist Arg;

private:
    _t _v;

public:
    constexpr Zobrist () : _v{0} {}
    constexpr explicit Zobrist (_t z) : _v(z) {}
    constexpr explicit operator _t () const { return _v; }

    void clear() { *this = Zobrist{}; }

    Zobrist& flip() { _v = ::bswap(_v); return *this; }
    Zobrist operator ~ () const { return Zobrist{*this}.flip(); }

    void drop(PieceType ty, Square to) { _v ^= key(ty, to); }
    void clear(PieceType ty, Square from) { drop(ty, from); }

    void setEnPassant(Square from) { drop( Pawn, Square(File(from), Rank8) ); }
    void clearEnPassant(Square from) { setEnPassant(from); }

    void setCastling(Square from) { drop( Pawn, Square(File(from), Rank1) ); }
    void clearCastling(Square from) { setCastling(from); }

    static Zobrist combine(Arg my, Arg op) { return Zobrist{my._v ^= static_cast<_t>(~op)}; }

    constexpr friend bool operator == (Arg a, Arg b) { return a._v == b._v; }
};

#endif
