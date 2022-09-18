#ifndef PI_SQUARE_HPP
#define PI_SQUARE_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Square.hpp"
#include "VectorPiEnum.hpp"
#include "VectorOf.hpp"

class PiSquare : public VectorPiEnum<Square::_t> {
    typedef VectorPiEnum<Square::_t> Base;

    bool none(Square sq) { assert (sq.isOk()); return (*this == sq).none(); }

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

    PiMask alivePieces() const { return notEmpty(); }
    PiMask piecesOn(Square sq) const { assert (sq.isOk()); return *this == sq; }
    PiMask leftForward(Square sq) const { assert (sq.isOk()); return *this < sq; }
    PiMask rightBackward(Square sq) const { assert (sq.isOk()); return *this > sq; }

    Square squareOf(Pi pi) const { assertValid(pi); return get(pi); }
    bool hasPieceOn(Square sq) const { assert (sq.isOk()); return piecesOn(sq).any(); }
    Pi pieceOn(Square sq) const { assert (sq.isOk()); assert (hasPieceOn(sq)); return piecesOn(sq).index(); }

    PiMask piecesOn(Rank rank) const {
        return _mm_cmpeq_epi8(
            static_cast<_t>(*this) & ::vectorOfAll[static_cast<unsigned char>(0xff^File::Mask)],
            ::vectorOfAll[static_cast<unsigned char>(rank << 3)]
        );
    }

    void clear(Pi pi) { assertValid(pi); Base::clear(pi); }

    void drop(Pi pi, Square sq) {
        assert (none(sq));
        assert (isEmpty(pi));
        Base::drop(pi, sq);
    }

    void move(Pi pi, Square sq) {
        assert (none(sq));
        assert (squareOf(pi) != sq);
        set(pi, sq);
    }

    void castle(Square kingTo, Pi rook, Square rookTo) {
        assert (!isEmpty(TheKing) && !isEmpty(rook));
        assert (TheKing != rook && kingTo != rookTo);
        assert (squareOf(TheKing).on(Rank1) && squareOf(rook).on(Rank1));
        assert (kingTo.on(Rank1) && rookTo.on(Rank1));

        set(TheKing, kingTo);
        set(rook, rookTo);
    }

};

#endif
