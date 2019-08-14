#ifndef VECTOR_PI_SQUARE_HPP
#define VECTOR_PI_SQUARE_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Square.hpp"
#include "VectorPiEnum.hpp"
#include "VectorOf.hpp"

class VectorPiSquare {
    VectorPiEnum<Square::_t> _v;
    typedef VectorPiEnum<Square::_t>::_t _t;

    bool isEmpty(Pi pi) const { return _v.isEmpty(pi); }
    bool none(Square sq) { assert (sq.isOk()); return (_v == sq).none(); }

public:

#ifdef NDEBUG
    void assertValid(Pi) const {}
#else
    void assertValid(Pi pi) const {
        assert (!isEmpty(pi));
        Square sq = _v[pi];
        assert (pieceOn(sq) == pi);
    }
#endif

    VectorPiMask alivePieces() const { return _v.notEmpty(); }
    VectorPiMask piecesOn(Square sq) const { return _v == sq; }
    VectorPiMask leftForward(Square sq) const { return _v < sq; }
    VectorPiMask rightBackward(Square sq) const { return _v > sq; }

    Square squareOf(Pi pi) const { assertValid(pi); return _v[pi]; }
    bool hasPieceOn(Square sq) const { return piecesOn(sq).any(); }
    Pi pieceOn(Square sq) const { assert (hasPieceOn(sq)); return piecesOn(sq).index(); }

    VectorPiMask piecesOn(Rank rank) const {
        return _mm_cmpeq_epi8(
            static_cast<_t>(_v) & ::vectorOfAll[static_cast<unsigned char>(0xff^File::Mask)],
            ::vectorOfAll[static_cast<unsigned char>(rank << 3)]
        );
    }

    void clear() { _v.clear(); }
    void clear(Pi pi) { assertValid(pi); _v.clear(pi); }

    void drop(Pi pi, Square sq) {
        assert (none(sq));
        assert (isEmpty(pi));
        _v.drop(pi, sq);
    }

    void move(Pi pi, Square sq) {
        assert (none(sq));
        assert (squareOf(pi) != sq);
        _v.set(pi, sq);
    }

    void castle(Square kingTo, Pi rook, Square rookTo) {
        assert (!isEmpty(TheKing) && !isEmpty(rook));
        assert (TheKing != rook && kingTo != rookTo);
        assert (squareOf(TheKing).on(Rank1) && squareOf(rook).on(Rank1));
        assert (kingTo.on(Rank1) && rookTo.on(Rank1));

        _v.set(TheKing, kingTo);
        _v.set(rook, rookTo);
    }

    friend io::ostream& operator << (io::ostream& out, VectorPiSquare squares) {
        auto flags = out.flags();

        out << std::hex;
        FOR_INDEX(Pi, pi) {
            out << ' ' << pi;
        }
        out << '\n';

        FOR_INDEX(Pi, pi) {
            out << std::setw(2) << static_cast<unsigned>(small_cast<unsigned char>( squares._v[pi] ));
        }
        out << '\n';

        out.flags(flags);
        return out;
    }

};

#endif
