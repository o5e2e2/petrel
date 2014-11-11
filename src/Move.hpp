#ifndef MOVE_HPP
#define MOVE_HPP

#include "Square.hpp"

/**
 * Internal move is packed to 12 bits and connected with the position from it was made
 * Independent move is extended to 13 bits (extra flag bit to mark castling, promotion or en passant move)
 **/
class Move {
    enum { FromShift, ToShift = FromShift+6, SpecialShift = ToShift+6 };
    enum Type { Simple = false, Special = true };

    typedef index_t _t;
    _t _v;

    constexpr Move (Square f, Square t, Type is_special)
        : _v(static_cast<_t>(f<<FromShift | t<<ToShift | is_special<<SpecialShift))
        {}

public:
    constexpr Move () : _v(0) {} //null move
    constexpr Move (Square f, Square t)
        : _v(static_cast<_t>(f<<FromShift | t<<ToShift))
        {}

    constexpr Move (Square f, Square t, PromoType ty)
        : Move(f, Square(File(t), static_cast<Rank::_t>(+ty)), Special)
        {}

    constexpr static Move null() { return Move{}; }
    constexpr static Move makeSpecial(Square f, Square t) { return Move(f, t, Special); }
    constexpr static Move makeCastling(Square f, Square t) { return Move(f, t, Special); }

    Square from() const { return static_cast<Square::_t>(_v >>FromShift & Square::Mask); }
    Square to() const { return static_cast<Square::_t>(_v >>ToShift & Square::Mask); }

    static PromoType decodePromoType(Square to) {
        return static_cast<PromoType::_t>(+Rank{to});
    }

    bool isNull() const { return _v == 0; }
    bool isSpecial() const { return (_v & 1<<SpecialShift) != 0; }

};

#endif
