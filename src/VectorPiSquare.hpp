#ifndef VECTOR_PI_SQUARE_HPP
#define VECTOR_PI_SQUARE_HPP

#include <iomanip>
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
    bool isOn(Square sq) const { return on(sq).any(); }
    Pi pieceOn(Square sq) const { assert (isOn(sq)); return on(sq).index(); }

    VectorPiMask alive() const { return _v.notEmpty(); }

    VectorPiMask on(Square sq) const { return _v == sq; }
    VectorPiMask leftForward(Square sq) const { return _v < sq; }
    VectorPiMask rightBackward(Square sq) const { return _v > sq; }

    template <Rank::_t Rank>
    VectorPiMask of() const {
        return _mm_cmpeq_epi8(static_cast<_t>(_v) & ::vectorOfAll[static_cast<unsigned char>(0xff^File::Mask)], ::vectorOfAll[Rank << 3]);
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

    friend std::ostream& operator << (std::ostream&, VectorPiSquare);

};

template <> inline VectorPiMask VectorPiSquare::of<Rank1>() const {
    //TRICK: rightBackward(A2) is equal to of<Rank1>(), but a bit faster
    return rightBackward(A2);
}

template <> inline VectorPiMask VectorPiSquare::of<Rank8>() const {
    //TRICK: leftForward(H7) is equal of<Rank8>(), but a bit faster
    return leftForward(H7);
}

std::ostream& operator << (std::ostream&, VectorPiSquare);

#endif
