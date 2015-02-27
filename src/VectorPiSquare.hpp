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

    Square squareOf(Pi pi) const { assertValid(pi); return _v[pi]; }
    bool isPieceOn(Square sq) const { return piecesOn(sq).any(); }
    Pi pieceOn(Square sq) const { assert (isPieceOn(sq)); return piecesOn(sq).index(); }

    VectorPiMask alivePieces() const { return _v.notEmpty(); }

    VectorPiMask piecesOn(Square sq) const { return _v == sq; }
    VectorPiMask leftForward(Square sq) const { return _v < sq; }
    VectorPiMask rightBackward(Square sq) const { return _v > sq; }

    VectorPiMask piecesOn(Rank rank) const {
        return _mm_cmpeq_epi8(static_cast<_t>(_v) & ::vectorOfAll[static_cast<unsigned char>(0xff^File::Mask)], ::vectorOfAll[rank << 3]);
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

    void castle(Pi p1, Square s1, Pi p2, Square s2) {
        assert (!isEmpty(p1) && !isEmpty(p2));
        assert (p1 != p2 && s1 != s2);

        _v.set(p1, s1);
        _v.set(p2, s2);
    }

    friend std::ostream& operator << (std::ostream& out, VectorPiSquare squares) {
        auto flags = out.flags();

        out << std::hex;
        FOR_INDEX(Pi, pi) {
            out << ' ' << pi;
        }
        out << '\n';

        FOR_INDEX(Pi, pi) {
            out << std::setw(2) << static_cast<unsigned>(small_cast<unsigned char>((squares._v[pi])));
        }
        out << '\n';

        out.flags(flags);
        return out;
    }
};

#endif
