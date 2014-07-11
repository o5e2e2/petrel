#ifndef VECTOR_PI_ORDER_HPP
#define VECTOR_PI_ORDER_HPP

#include "Vector.hpp"
#include "Index.hpp"
#include "BitSet.hpp"
#include "VectorPiEnum.hpp"
#include "VectorPiSingle.hpp"

class VectorPiOrder {
    typedef __m128i _t;
    static const _t sorted;

    VectorPiEnum<Pi::_t> _v;

    bool isOk() const {
        _t a = _mm_shuffle_epi8(static_cast<_t>(_v), static_cast<_t>(_v));
        return _mm_movemask_epi8(_mm_cmpeq_epi8(a, sorted)) == 0xffffu;
    }

public:
    VectorPiOrder () : _v(sorted) {}
    //VectorPiOrder (_t v) : _v(v) { assert (isOk()); }

    VectorPiMask order(VectorPiMask a) const {
        return _mm_shuffle_epi8(static_cast<VectorPiMask::_t>(a), static_cast<_t>(_v));
    }
    Pi operator[] (Pi pi) const { return _v[pi]; }

};

/*
class PiOrderedIterator {
    PieceSet mask;
    VectorPiOrder order;

public:
    PiOrderedIterator (VectorPiMask m, VectorPiOrder o)
        : mask(o.order(m)), order(o) {}

    Pi operator * () const { return order[*mask]; }
    PiOrderedIterator& operator ++ () { ++mask; return *this; }
    PiOrderedIterator begin() const { return *this; }
    //PiOrderedIterator end() const { return PiOrderedIterator {0, order}; }
};
*/
#endif
