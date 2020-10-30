#ifndef MOVE_HPP
#define MOVE_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Square.hpp"

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
    enum { FromShift, ToShift = FromShift+6, SpecialShift = ToShift+6 };
    enum Type { Simple = false, Special = true };

    typedef index_t _t;
    _t _v;

    constexpr Move (Square f, Square t, Type is_special) : _v{static_cast<_t>(f<<FromShift | t<<ToShift | is_special<<SpecialShift)} {}
    constexpr static Move special(Square f, Square t) { return Move{f, t, Special}; }

    constexpr bool isSpecial() const { return (_v & 1<<SpecialShift) != 0; }

public:
    constexpr Move () : _v{0} {} //null move
    constexpr Move (Square f, Square t) : _v{static_cast<_t>(f<<FromShift | t<<ToShift)} {}
    constexpr Move operator ~ () const { return Move{~from(), ~to(), static_cast<Move::Type>(isSpecial())}; }

    constexpr static Move enPassant(Square f, Square t) { return Move::special(f, t); }
    constexpr static Move castling(Square f, Square t)  { return Move::special(f, t); }
    constexpr static Move promotion(Square f, Square t) { return Move::special(f, t); }
    constexpr static Move promotion(Square f, Square t, PromoType ty) {
        return Move::promotion(f, Square{File(t), static_cast<Rank::_t>(+ty)});
    }

    constexpr operator bool() const { return _v != 0; }

    constexpr Square from() const { return static_cast<Square::_t>(_v >>FromShift & Square::Mask); }
    constexpr Square to()   const { return static_cast<Square::_t>(_v >>ToShift & Square::Mask); }
    constexpr PromoType promoType() const { return PromoType(to()); }

    static io::ostream& write(io::ostream&, Move, Color, ChessVariant = Orthodox);
    static io::ostream& write(io::ostream&, const Move[], Color, ChessVariant = Orthodox);
};

#endif
