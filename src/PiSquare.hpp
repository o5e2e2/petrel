#ifndef PI_SQUARE_HPP
#define PI_SQUARE_HPP

#include "typedefs.hpp"
#include "Square.hpp"
#include "PiMask.hpp"
#include "PiVector.hpp"
#include "VectorOf.hpp"

class PiSquare : protected PiVector {
    enum { None = 0xff };

    PiMask piecesOn(Square sq) const { return PiMask(v, ::vectorOfAll[sq]); }
    bool none(Square sq) { return piecesOn(sq).none(); }

    PiMask notEmpty() const { return PiMask::cmpgt(v, ::vectorOfAll[None]); }
    bool isEmpty(Pi pi) const { return get(pi) == None; }

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert (!isEmpty(pi));
        assert (pieceOn(static_cast<Square::_t>(get(pi))) == pi);
    }
#endif

public:
    constexpr PiSquare () : PiVector(::vectorOfAll[None]) {}

    PiMask pieces() const { return notEmpty(); }
    PiMask piecesOn(Rank rank) const { return PiMask( v & ::vectorOfAll[0xff ^ File::Mask], ::vectorOfAll[rank << 3]); }

    Square squareOf(Pi pi) const { assertValid(pi); return static_cast<Square::_t>(get(pi)); }
    bool has(Square sq) const { return piecesOn(sq).any(); }
    Pi pieceOn(Square sq) const { assert (has(sq)); return piecesOn(sq).index(); }

    void clear(Pi pi) { assertValid(pi); set(pi, None); }
    void drop(Pi pi, Square sq) { assert (isEmpty(pi)); assert (none(sq)); set(pi, sq); }
    void move(Pi pi, Square sq) { assertValid(pi); assert (none(sq)); set(pi, sq); }

    void castle(Square kingTo, Pi theRook, Square rookTo) {
        assert (TheKing != theRook);
        assertValid(TheKing);
        assertValid(theRook);

        assert (squareOf(TheKing).on(Rank1));
        assert (squareOf(theRook).on(Rank1));

        assert (kingTo.is(G1) || kingTo.is(C1));
        assert (rookTo.is(F1) || rookTo.is(D1));

        set(TheKing, kingTo);
        set(theRook, rookTo);

        assertValid(TheKing);
        assertValid(theRook);
    }

};

#endif
