#ifndef ZOBRIST_KEY_HPP
#define ZOBRIST_KEY_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class ZobristKey {
public:
    typedef std::uint64_t _t;
    ZobristKey ();
    const _t& operator() (PieceTag ty, Square sq) const { return key[ty][sq]; }

private:
    _t key[PieceTag::Size][Square::Size];

    constexpr static _t r(_t n, Square::_t sq) { return n << sq | n >> (64-sq); }

};

extern const ZobristKey zobristKey;

#endif
