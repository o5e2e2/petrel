#ifndef MOVE_HPP
#define MOVE_HPP

#include "io.hpp"
#include "Square.hpp"

enum ChessVariant { Orthodox, Chess960 };

/**
 * Internal move is packed to 12 bits and connected with the position from it was made
 * Independent move is extended to 13 bits (extra flag bit to mark castling, promotion or en passant move)
 **/
class Move {
    enum { FromShift, ToShift = FromShift+6, SpecialShift = ToShift+6 };

    typedef index_t _t;
    _t _v;

public:
    enum Type { Simple = false, Special = true };
    Move () : _v(0) {} //null move
    Move (Square f, Square t, Type is_special = Simple)
        : _v(static_cast<_t>(f<<FromShift | t<<ToShift | is_special<<SpecialShift))
        {}

    Square from() const { return static_cast<Square::_t>(_v >>FromShift & Square::Mask); }
    Square to() const { return static_cast<Square::_t>(_v >>ToShift & Square::Mask); }

    bool isNull() const { return _v == 0; }
    bool isSpecial() const { return (_v & 1<<SpecialShift) != 0; }
};

std::ostream& write(std::ostream&, Move, Color, ChessVariant);

#endif
