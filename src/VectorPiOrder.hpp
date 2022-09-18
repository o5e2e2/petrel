#ifndef VECTOR_PI_ORDER_HPP
#define VECTOR_PI_ORDER_HPP

#include "typedefs.hpp"
#include "VectorPiEnum.hpp"
#include "VectorPiSingle.hpp"

class VectorPiOrder {
    typedef __m128i _t;

    struct Sorted {
        union {
            _t _v;
            char a[16];
        };
        constexpr Sorted () : a{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} {}
        constexpr operator _t () const { return _v; }
    };
    static const Sorted sorted;

    VectorPiEnum<Pi::_t> _v;

    bool isOk() const {
        _t a = _mm_shuffle_epi8(static_cast<_t>(_v), static_cast<_t>(_v));
        return _mm_movemask_epi8(_mm_cmpeq_epi8(a, sorted)) == 0xffffu;
    }

public:
    VectorPiOrder () : _v(sorted) {}
    VectorPiOrder (_t v) : _v(v) { assert (isOk()); }

    PiMask order(PiMask a) const {
        return _mm_shuffle_epi8(static_cast<PiMask::_t>(a), static_cast<_t>(_v));
    }
    Pi operator[] (Pi pi) const { return _v[pi]; }

};

class PiOrderedIterator {
    VectorPiOrder order;
    PieceSet mask;

public:
    PiOrderedIterator (VectorPiOrder o, PiMask m)
        : order(o), mask(o.order(m)) {}

    Pi operator * () const { return order[*mask]; }
    PiOrderedIterator& operator ++ () { ++mask; return *this; }
    PiOrderedIterator begin() const { return *this; }
    //PiOrderedIterator end() const { return PiOrderedIterator(order, 0); }
};

#endif
