#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <random>
#include "typedefs.hpp"
#include "Square.hpp"

class Zobrist {
    class Key {
    public:
        typedef std::uint64_t _t;

    private:
        _t key[PieceType::Size][Square::Size];

        constexpr static _t r(_t n, index_t i) { return n << i | n >> (64-i); }

    public:
        Key () {
//#define RANDOM_KEYS
#ifdef RANDOM_KEYS
            std::mt19937_64 random;
            FOR_INDEX(PieceType, ty) {
                FOR_INDEX(Square, sq) {
                    key[ty][sq] = random();
                }
            }
#else
            _t init[PieceType::Size] = {
                0x0218a392d367abbfull,
                0x0218fd49de59b457ull,
                0x021b2a4fd16bc773ull,
                0x0323dba73562fc25ull,
                0x032fc73dbac2a4d1ull,
                0x03422eadec73253full,
            };

            FOR_INDEX(PieceType, ty) {
                FOR_INDEX(Square, sq) {
                    key[ty][sq] = r(init[ty], sq);
                }
            }

            FOR_INDEX(File, file) {
                _t extra = 0x026763d5c37e5a45ull;
                key[Pawn][Square(file, Rank8)] = r(extra ^ init[Pawn], Square(file, Rank4)); //en passant
                key[Pawn][Square(file, Rank1)] = r(extra ^ init[Rook], Square(file, Rank1)); //castling
            }
#endif
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
    constexpr operator const _t& () const { return _v; }

    void clear() { *this = {}; }

    Zobrist& flip() { _v = ~::bswap(_v); return *this; }
    Zobrist operator ~ () const { return Zobrist{*this}.flip(); }

    void drop(PieceType ty, Square to) { _v ^= key(ty, to); }
    void clear(PieceType ty, Square from) { drop(ty, from); }

    void setEnPassant(File fileFrom) { drop( Pawn, Square(fileFrom, Rank8) ); }
    void clearEnPassant(File fileFrom) { setEnPassant(fileFrom); }

    void setCastling(Square from) { assert (from.is(Rank1)); drop(Pawn, from); }
    void clearCastling(Square from) { setCastling(from); }

    static Zobrist combine(Arg my, Arg op) { return Zobrist{my._v ^= static_cast<_t>(~op)}; }

    constexpr friend bool operator == (Arg a, Arg b) { return a._v == b._v; }
};

#endif
