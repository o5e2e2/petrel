#ifndef VECTOR_PI_TRAIT_H
#define VECTOR_PI_TRAIT_H

#include "typedefs.hpp"
#include "VectorPiBit.hpp"

class VectorPiTrait {
    enum pi_trait_t {
        Castling,  // rooks with castling rights
        EnPassant, // pawns that can either be legally captured en passant or perform a legal en passant capture (depends on side to move)
        Pinner,    // sliding pieces that can attack the enemy king on empty board (potential pinners)
        Checker,   // any piece actually attacking enemy king
    };

    typedef ::Index<4, pi_trait_t> Index;
    struct Value : VectorPiBit<Value, Index> {};

    Value _v;

public:
    void clear(Pi pi) { _v.clear(pi); }

    VectorPiMask castlingRooks() const { return _v.anyOf(Castling); }
    bool isCastling(Pi pi) const { return _v.is(pi, Castling); }
    void setCastling(Pi pi) { assert (!isCastling(pi)); _v.set(pi, Castling); }
    void clearCastlings() { _v.clear(Castling); }

    VectorPiMask enPassantPawns() const { return _v.anyOf(EnPassant); }
    Pi getEnPassant() const { Pi pi = enPassantPawns().index(); return pi; }
    bool isEnPassant(Pi pi) const { return _v.is(pi, EnPassant); }
    void setEnPassant(Pi pi) { _v.set(pi, EnPassant); }
    void clearEnPassant(Pi pi) { assert (isEnPassant(pi)); _v.clear(pi, EnPassant); }
    void clearEnPassants() { _v.clear(EnPassant); }

    VectorPiMask pinners() const { return _v.anyOf(Pinner); }
    void clearPinners() { _v.clear(Pinner); }
    void setPinner(Pi pi) { assert (!_v.is(pi, Pinner)); _v.set(pi, Pinner); }
    void clearPinner(Pi pi) { _v.clear(pi, Pinner); }

    VectorPiMask checkers() const { return _v.anyOf(Checker); }
    void clearCheckers() { _v.clear(Checker); }
    void setChecker(Pi pi) { assert (!_v.is(pi, Checker)); _v.set(pi, Checker); }
};

#endif
