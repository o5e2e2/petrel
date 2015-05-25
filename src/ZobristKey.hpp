#ifndef ZOBRIST_KEY_HPP
#define ZOBRIST_KEY_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class ZobristKey {
public:
    typedef std::uint64_t _t;

private:
    _t key[PieceTag::Size][Square::Size];

public:
    ZobristKey ();
    const _t& operator() (PieceTag ty, Square sq) const { return key[ty][sq]; }

};

extern const ZobristKey zobristKey;

#endif
