#ifndef VECTOR_PI_TYPE_H
#define VECTOR_PI_TYPE_H

#include "Index.hpp"
#include "VectorPiBit.hpp"

class VectorPiType {
    //Castling: rooks with castling rights;
    //EnPassant: pawns that can either be legally captured en passant or perform a legal en passant capture (depends on side to move);
    //PinRay: sliders that would check the enemy king location on the empty board
    //TRICK: EnPassant and Castling flags unioned into one bit field

    //Queen, Rook, Bishop, Knight, Pawn, King = 0..5
    enum piece_tag_t { Castling=6, EnPassant=Castling, PinRay };
    typedef Index<8, piece_type_t> PieceTag;

    struct Value : VectorPiBit<Value, PieceTag> {};
    typedef Value::element_type _t;

    enum { SliderMask = ::singleton<_t>(Queen)|::singleton<_t>(Rook)|::singleton<_t>(Bishop) };
    enum { EnPassantMask = ::singleton<_t>(EnPassant)|::singleton<_t>(Pawn) };
    enum { CastlingMask = ::singleton<_t>(Castling)|::singleton<_t>(Rook) };

    Value _v;
    bool isEmpty(Pi pi) const { return _v.isEmpty(pi); }

    template <piece_tag_t Type> VectorPiMask of() const { return _v.get<static_cast<PieceType::_t>(Type)>(); }
    template <piece_tag_t Type> bool is(Pi pi) const { return _v.is<static_cast<PieceType::_t>(Type)>(pi); }

    template <piece_tag_t Type> void set(Pi pi) { _v.set<static_cast<PieceType::_t>(Type)>(pi); }
    template <piece_tag_t Type> void clear(Pi pi) { _v.clear<static_cast<PieceType::_t>(Type)>(pi); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert (!isEmpty(pi));
        assert (_v.is<King>(pi) == (pi == TheKing));
    }
#endif
    VectorPiMask alive() const { return _v.notEmpty(); }

    PieceType typeOf(Pi pi) const {
        assert (static_cast<PieceType::_t>(Castling) > Rook);
        assertValid(pi);
        PieceType ty{static_cast<PieceType::_t>( ::bsf(static_cast<U32>(_v[pi])) )};
        assert (ty.isOk());
        return ty;
    }

    template <PieceType::_t Type> bool is(Pi pi) const { assertValid(pi); return _v.is<Type>(pi); }
    template <PieceType::_t Type> VectorPiMask of() const { return _v.get<Type>(); }

    bool isSlider(Pi pi) const { assertValid(pi); return (_v[pi] & SliderMask) != 0; }
    VectorPiMask sliders() const { return _v.getAnyMask<SliderMask>(); }

    VectorPiMask castlings() const { return _v.getAllMask<CastlingMask>(); }
    bool isCastling(Pi pi) const { assert (isSlider(pi)); return is<Castling>(pi); }
    void setCastling(Pi pi) { assert (is<Rook>(pi)); assert (!isCastling(pi)); set<Castling>(pi); }
    void clearCastling(Pi pi) { assert (is<Rook>(pi)); assert (isCastling(pi)); clear<Castling>(pi); }

    VectorPiMask enPassants() const { return _v.getAllMask<EnPassantMask>(); }
    bool isEnPassant(Pi pi) const { return is<Pawn>(pi) && is<EnPassant>(pi); }
    void setEnPassant(Pi pi) { assert (is<Pawn>(pi)); assert (!isEnPassant(pi)); set<EnPassant>(pi); }
    void clearEnPassant(Pi pi) { assert (is<Pawn>(pi)); assert (isEnPassant(pi)); clear<EnPassant>(pi); }
    void clearEnPassant() { _v.clearMasked<static_cast<PieceType::_t>(EnPassant), Pawn>(); }

    VectorPiMask pinnerCandidates() const { return of<PinRay>(); }
    void clearPinRay(Pi pi) { assert (isSlider(pi)); clear<PinRay>(pi); }
    void setPinRay(Pi pi) { assert (isSlider(pi)); set<PinRay>(pi); }

    void clear() { _v.clear(); }
    void clear(Pi pi) { assert (!is<King>(pi)); _v.clear(pi); }
    void drop(Pi pi, PieceType ty) { assert (isEmpty(pi)); _v.set(pi, ty); }
    void promote(Pi pi, PromoType ty) { assert (is<Pawn>(pi)); _v.set(pi, ty); }

};

#endif
