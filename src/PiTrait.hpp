#ifndef PI_TRAIT_H
#define PI_TRAIT_H

#include "typedefs.hpp"
#include "PiBit.hpp"

enum pi_trait_t {
    Castling,  // rooks with castling rights
    EnPassant, // pawn can be captured en passant or pawn or pawns that can perform a legal en passant capture (depends on side to move)
    Pinner,    // sliding pieces that can attack the enemy king on empty board (potential pinners)
    Checker,   // any piece actually attacking enemy king
};

class PiTrait : protected PiBit< PiTrait, Index<4, pi_trait_t> > {
    typedef PiBit< PiTrait, Index<4, pi_trait_t> > Base;
public:
    using Base::clear;
    using Base::isEmpty;

    PiMask castlingRooks() const { return anyOf(Castling); }
    bool isCastling(Pi pi) const { return is(pi, Castling); }
    void setCastling(Pi pi) { assert (!isCastling(pi)); set(pi, Castling); }
    void clearCastlings() { clear(Castling); }

    PiMask enPassantPawns() const { return anyOf(EnPassant); }
    Pi getEnPassant() const { Pi pi = enPassantPawns().index(); return pi; }
    bool isEnPassant(Pi pi) const { return is(pi, EnPassant); }
    void setEnPassant(Pi pi) { set(pi, EnPassant); }
    void clearEnPassant(Pi pi) { assert (isEnPassant(pi)); clear(pi, EnPassant); }
    void clearEnPassants() { clear(EnPassant); }

    PiMask pinners() const { return anyOf(Pinner); }
    bool isPinner(Pi pi) const { return is(pi, Pinner); }
    void clearPinners() { clear(Pinner); }
    void setPinner(Pi pi) { assert (!isPinner(pi)); set(pi, Pinner); }
    void clearPinner(Pi pi) { clear(pi, Pinner); }

    PiMask checkers() const { return anyOf(Checker); }
    void clearCheckers() { clear(Checker); }
    void setChecker(Pi pi) { assert (!is(pi, Checker)); set(pi, Checker); }
};

#endif
