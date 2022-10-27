#ifndef PI_SQUARE_HPP
#define PI_SQUARE_HPP

#include "typedefs.hpp"
#include "Square.hpp"
#include "VectorPiEnum.hpp"
#include "PiMask.hpp"
#include "VectorOf.hpp"

class PiSquare : protected VectorPiEnum<Square::_t> {
    typedef VectorPiEnum<Square::_t> Base;

    bool none(Square sq) { return (*this == sq).none(); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert (!isEmpty(pi));
        Square sq = get(pi);
        assert (pieceOn(sq) == pi);
    }
#endif

    PiMask pieces() const { return notEmpty(); }
    PiMask piecesOn(Square sq) const { return *this == sq; }
    PiMask leftForward(Square sq) const { return *this < sq; }
    PiMask rightBackward(Square sq) const { return *this > sq; }

    Square squareOf(Pi pi) const { assertValid(pi); return get(pi); }
    bool hasPieceOn(Square sq) const { return piecesOn(sq).any(); }
    Pi pieceOn(Square sq) const { assert (hasPieceOn(sq)); return piecesOn(sq).index(); }

    PiMask piecesOn(Rank rank) const {
        return PiMask::cmpeq(
            static_cast<_t>(*this) & ::vectorOfAll[static_cast<unsigned char>(0xff^File::Mask)],
            ::vectorOfAll[static_cast<unsigned char>(rank << 3)]
        );
    }

    void clear(Pi pi) {
        assertValid(pi);
        Base::clear(pi);
        assert (isEmpty(pi));
    }

    void drop(Pi pi, Square sq) {
        assert (isEmpty(pi));
        assert (none(sq));
        set(pi, sq);
        assertValid(pi);
    }

    void move(Pi pi, Square sq) {
        assertValid(pi);
        assert (none(sq));
        set(pi, sq);
        assertValid(pi);
    }

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
