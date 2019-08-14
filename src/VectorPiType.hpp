#ifndef VECTOR_PI_TYPE_H
#define VECTOR_PI_TYPE_H

#include "typedefs.hpp"
#include "VectorPiBit.hpp"

class VectorPiType {
    //Castling: rooks with castling rights
    //EnPassant: pawns that can either be legally captured en passant or perform a legal en passant capture (depends on side to move)
    //Pinner: sliding pieces that can attack the enemy king on empty board

    typedef ::Index<8, piece_type_t> Index;

    struct Value : VectorPiBit<Value, Index> {};
    typedef Value::element_type _t;

    enum {
        SliderMask = ::singleton<_t>(Queen)|::singleton<_t>(Rook)|::singleton<_t>(Bishop),
        LeaperMask = ::singleton<_t>(King)|::singleton<_t>(Knight)|::singleton<_t>(Pawn),
        MinorMask = ::singleton<_t>(Bishop)|::singleton<_t>(Knight),
        PieceTypeMask = SliderMask|LeaperMask
    };

    Value _v;
    bool isEmpty(Pi pi) const { return _v.isEmpty(pi); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert ( !isEmpty(pi) );
        assert ( _v.is(pi, King) || ::isSingleton(_v[pi] & PieceTypeMask) );
        assert ( !_v.is(pi, King) || (pi.is(TheKing) && !_v.is(pi, Rook) && !_v.is(pi, Pawn)) || _v.is(pi, Rook) || _v.is(pi, Pawn) );
    }
#endif

    VectorPiMask alivePieces() const { return _v.notEmpty(); }
    VectorPiMask piecesOfType(PieceType ty) const { assert (!ty.is(King)); return _v.anyOf(ty); }
    VectorPiMask minors() const { return _v.anyOf(MinorMask); }
    VectorPiMask sliders() const { return _v.anyOf(SliderMask); }
    VectorPiMask castlingRooks() const { return _v.anyOf(Castling); }
    VectorPiMask enPassantPawns() const { return _v.anyOf(EnPassant) & _v.anyOf(Pawn); }
    VectorPiMask pinners() const { return _v.anyOf(Pinner) & _v.anyOf(SliderMask); }

    PieceType typeOf(Pi pi) const { assertValid(pi); return static_cast<PieceType::_t>( ::bsf(static_cast<unsigned>(_v[pi])) ); }
    Pi getEnPassant() const { Pi pi = enPassantPawns().index(); assert (isPawn(pi)); return pi; }

    bool isOfType(Pi pi, PieceType ty) const { assert (!ty.is(King)); assertValid(pi); return _v.is(pi, ty); }
    bool isPawn(Pi pi) const { return isOfType(pi, Pawn); }
    bool isCastling(Pi pi) const { return isOfType(pi, Castling); }
    bool isEnPassant(Pi pi) const { assert (isPawn(pi)); return isOfType(pi, EnPassant); }
    bool isSlider(Pi pi) const { assertValid(pi); return (_v[pi] & SliderMask) != 0; }

    void setCastling(Pi pi) { assert (isOfType(pi, Rook)); assert (!isCastling(pi)); _v.set(pi, Castling); }
    void clearCastling(Pi pi) { assert (!isCastling(pi) || isOfType(pi, Rook)); _v.clear(pi, Castling); }
    void clearCastlings() { _v.clear(Castling); }

    void setEnPassant(Pi pi) { assert (isPawn(pi)); _v.set(pi, EnPassant); }
    void clearEnPassant(Pi pi) { assert (isEnPassant(pi)); _v.clear(pi, EnPassant); }
    void clearEnPassants() { _v.clearIf(EnPassant, Pawn); }

    void setPinner(Pi pi) { assert (isSlider(pi)); _v.set(pi, Pinner); }
    void clearPinner(Pi pi) { assert (isSlider(pi)); _v.clear(pi, Pinner); }

    void clear() { _v.clear(); }
    void clear(Pi pi) { assert (!_v.is(pi, King) || _v.is(pi, Rook) || _v.is(pi, Pawn)); _v.clear(pi); }
    void drop(Pi pi, PieceType ty) { assert (isEmpty(pi)); _v.set(pi, ty); }
    void promote(Pi pi, PromoType ty) { assert (isPawn(pi)); _v.clear(pi); _v.set(pi, ty); }
};

#endif
