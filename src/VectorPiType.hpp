#ifndef VECTOR_PI_TYPE_H
#define VECTOR_PI_TYPE_H

#include "typedefs.hpp"
#include "VectorPiBit.hpp"

class VectorPiType {
    struct Value : VectorPiBit<Value, PieceType> {};

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
    VectorPiMask leapers() const { return _v.anyOf(LeaperMask); }

    PieceType typeOf(Pi pi) const { assertValid(pi); return static_cast<PieceType::_t>( ::bsf(static_cast<unsigned>(_v[pi])) ); }

    bool isOfType(Pi pi, PieceType ty) const { assert (!ty.is(King)); assertValid(pi); return _v.is(pi, ty); }
    bool isPawn(Pi pi) const { return isOfType(pi, Pawn); }
    bool isSlider(Pi pi) const { assertValid(pi); return (_v[pi] & SliderMask) != 0; }

    void clear() { _v.clear(); }
    void clear(Pi pi) { assert (!_v.is(pi, King) || _v.is(pi, Rook) || _v.is(pi, Pawn)); _v.clear(pi); }
    void drop(Pi pi, PieceType ty) { assert (isEmpty(pi)); _v.set(pi, ty); }
    void promote(Pi pi, PromoType ty) { assert (isPawn(pi)); _v.clear(pi); _v.set(pi, ty); }
};

#endif
