#ifndef MOVE_HPP
#define MOVE_HPP

#include "typedefs.hpp"
#include "Square.hpp"

class Position;

/**
 * Internal move is packed to 12 bits and connected with the position from it was made
 * Independent move is extended to 13 bits (extra flag bit to mark castling, promotion or en passant move)
 * All moves are relative to its side.
 *
 * Castling encoded as the castling rook moves over own king square.
 * Pawn promotion piece encoded in place of destination rank.
 * En passant capture encoded as the pawn moves over captured pawn square.
 **/
class Move {
    friend class Position;

    enum { FromShift, ToShift = FromShift+6, SpecialShift = ToShift+6 };
    enum Type { Simple = false, Special = true };

    typedef index_t _t;
    _t _v;

    constexpr Move (Square f, Square t) : _v(static_cast<_t>(f<<FromShift | t<<ToShift)) {}
    constexpr Move (Square f, Square t, Type is_special) : _v(static_cast<_t>(f<<FromShift | t<<ToShift | is_special<<SpecialShift)) {}

    constexpr static Move promotion(Square f, Square t, PromoType ty) { return Move{f, Square{File(t), static_cast<Rank::_t>(+ty)}, Special}; }
    constexpr static Move special(Square f, Square t) { return Move{f, t, Special}; }
    constexpr static Move enPassant(Square f, Square t) { return Move::special(f, t); }
    constexpr static Move castling(Square f, Square t)  { return Move::special(f, t); }

    constexpr static PromoType decodePromoType(Square to) { return static_cast<PromoType::_t>(+Rank{to}); }

    constexpr bool isNull() const { return _v == 0; }
    constexpr bool isSpecial() const { return (_v & 1<<SpecialShift) != 0; }

public:
    constexpr Move () : _v(0) {} //null move

    constexpr Square from() const { return static_cast<Square::_t>(_v >>FromShift & Square::Mask); }
    constexpr Square to()   const { return static_cast<Square::_t>(_v >>ToShift & Square::Mask); }

    static std::ostream& write(std::ostream&, Move, Color, ChessVariant);
};

#endif
