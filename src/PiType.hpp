#ifndef PI_TYPE_H
#define PI_TYPE_H

#include "typedefs.hpp"
#include "VectorPiBit.hpp"

class PiType : protected VectorPiBit<PiType, PieceType> {
    typedef VectorPiBit<PiType, PieceType> Base;

    typedef PieceType::_t _t;
    enum {
        Slider = ::singleton<_t>(Queen)|::singleton<_t>(Rook)|::singleton<_t>(Bishop),
        Leaper = ::singleton<_t>(King)|::singleton<_t>(Knight)|::singleton<_t>(Pawn),
        Minor  = ::singleton<_t>(Bishop)|::singleton<_t>(Knight),
    };

    bool is(Pi pi, PieceType ty) const { assertValid(pi); assert (!ty.is(King)); return Base::is(pi, ty); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert ( !isEmpty(pi) );
        assert ( ::isSingleton(get(pi)) );
    }
#endif

    PiMask alivePieces() const { return notEmpty(); }
    PiMask piecesOfType(PieceType ty) const { assert (!ty.is(King)); return anyOf(ty); }
    PiMask minors() const { return anyOf(Minor); }
    PiMask sliders() const { return anyOf(Slider); }
    PiMask leapers() const { return anyOf(Leaper); }

    PieceType typeOf(Pi pi) const { assertValid(pi); return static_cast<PieceType::_t>( ::bsf(static_cast<unsigned>(get(pi))) ); }

    bool isPawn(Pi pi) const { return is(pi, Pawn); }
    bool isRook(Pi pi) const { return is(pi, Rook); }
    bool isSlider(Pi pi) const { assertValid(pi); return (get(pi) & Slider) != 0; }

    void clear(Pi pi) { assertValid(pi); assert (pi != TheKing); assert (!Base::is(pi, King)); Base::clear(pi); }
    void drop(Pi pi, PieceType ty) { assert (isEmpty(pi)); assert (pi != TheKing || ty == King); set(pi, ty); }
    void promote(Pi pi, PromoType ty) { assert (isPawn(pi)); clear(pi); set(pi, ty); }
};

#endif
