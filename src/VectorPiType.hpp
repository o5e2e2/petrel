#ifndef VECTOR_PI_TYPE_H
#define VECTOR_PI_TYPE_H

#include "typedefs.hpp"
#include "VectorPiBit.hpp"

class VectorPiType {
    //Castling: rooks with castling rights;
    //EnPassant: pawns that can either be legally captured en passant or perform a legal en passant capture (depends on side to move);
    //PinRay: sliders that would check the enemy king location on the empty board
    //TRICK: EnPassant and Castling flags unioned into one bit field

    enum { SpecialPiece = King + 1, PinRay };
    typedef ::Index<8, piece_type_t> Index;

    struct Value : VectorPiBit<Value, Index> {};
    typedef Value::element_type _t;

    enum {
        SliderMask = ::singleton<_t>(Queen)|::singleton<_t>(Rook)|::singleton<_t>(Bishop),
        LeaperMask = ::singleton<_t>(King)|::singleton<_t>(Knight)|::singleton<_t>(Pawn),
        PieceTypeMask = SliderMask|LeaperMask,
        CastlingMask  = ::singleton<_t>(SpecialPiece)|::singleton<_t>(Rook),
        EnPassantMask = ::singleton<_t>(SpecialPiece)|::singleton<_t>(Pawn)
    };

    Value _v;
    bool isEmpty(Pi pi) const { return _v.isEmpty(pi); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert ( !isEmpty(pi) );
        assert ( _v.is(pi, King) || ::is_singleton(_v[pi] & PieceTypeMask) );
        assert ( !_v.is(pi, PinRay) || ((_v[pi] & SliderMask) != 0) );
        assert ( !_v.is(pi, King) || (pi.is(TheKing) && !_v.is(pi, Rook) && !_v.is(pi, Pawn)) || _v.is(pi, Rook) || _v.is(pi, Pawn) );
    }
#endif

    VectorPiMask alivePieces() const { return _v.notEmpty(); }
    VectorPiMask sliders() const { return _v.anyOf(SliderMask); }
    bool isSlider(Pi pi) const { assertValid(pi); return (_v[pi] & SliderMask) != 0; }

    VectorPiMask piecesOfType(PieceType ty) const { assert (!ty.is(King)); return _v.anyOf(ty); }
    bool isTypeOf(Pi pi, PieceType ty) const { assert (!ty.is(King)); assertValid(pi); return _v.is(pi, ty); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return static_cast<PieceType::_t>( ::bsf(static_cast<unsigned>(_v[pi])) ); }
    bool isPawn(Pi pi) const { return isTypeOf(pi, Pawn); }

    VectorPiMask castlingRooks() const { return _v.allOf(CastlingMask); }
    bool isCastling(Pi pi) const { return isTypeOf(pi, Rook) && _v.is(pi, static_cast<Index::_t>(SpecialPiece)); }
    void setCastling(Pi pi) { assert (isTypeOf(pi, Rook)); assert (!isCastling(pi)); _v.set(pi, static_cast<Index::_t>(SpecialPiece)); }
    void clearCastling(Pi pi) { assert (isCastling(pi)); _v.clear(pi, static_cast<Index::_t>(SpecialPiece)); }
    void clearCastlings() { _v.clearIf(static_cast<Index::_t>(SpecialPiece), Rook); }

    VectorPiMask enPassantPawns() const { return _v.allOf(EnPassantMask); }
    Pi   getEnPassant() const { Pi pi{ enPassantPawns().index() }; assert (isPawn(pi)); return pi; }
    bool isEnPassant(Pi pi) const { return isPawn(pi) && _v.is(pi, SpecialPiece); }
    void setEnPassant(Pi pi) { assert (isPawn(pi)); assert (!isEnPassant(pi)); _v.set(pi, static_cast<Index::_t>(SpecialPiece)); }
    void clearEnPassant(Pi pi) { assert (isEnPassant(pi)); _v.clear(pi, static_cast<Index::_t>(SpecialPiece)); }
    void clearEnPassants() { _v.clearIf(static_cast<Index::_t>(SpecialPiece), Pawn); }

    VectorPiMask pinnerCandidates() const { return _v.anyOf(static_cast<Index::_t>(PinRay)); }
    void setPinRay(Pi pi) { assert (isSlider(pi)); _v.set(pi, static_cast<Index::_t>(PinRay)); }
    void clearPinRay(Pi pi) { assert (isSlider(pi)); _v.clear(pi, static_cast<Index::_t>(PinRay)); }

    void clear() { _v.clear(); }
    void clear(Pi pi) { assert (!_v.is(pi, King) || _v.is(pi, Rook) || _v.is(pi, Pawn)); _v.clear(pi); }
    void drop(Pi pi, PieceType ty) { assert (isEmpty(pi)); _v.set(pi, ty); }
    void promote(Pi pi, PromoType ty) { assert (isPawn(pi)); _v.clear(pi); _v.set(pi, ty); }
};

#endif
