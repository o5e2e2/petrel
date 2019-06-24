#ifndef VECTOR_PI_MASK_HPP
#define VECTOR_PI_MASK_HPP

#include "io.hpp"
#include "typedefs.hpp"

#include "BitArray.hpp"
#include "PieceSet.hpp"
#include "VectorOf.hpp"
#include "VectorPiSingle.hpp"

/**
 * piece vector of boolean values: false (0) or true (0xff)
 */
class VectorPiMask : public BitArray<VectorPiMask, __m128i> {
    bool isOk() const {
        _t v = _mm_cmpgt_epi8(::vectorOfAll[0], this->_v);
        return _mm_movemask_epi8(_mm_cmpeq_epi8(this->_v, v)) == 0xffffu;
    }

public:
    VectorPiMask (_t v) : BitArray{v} { assert (isOk()); }
    VectorPiMask (Pi pi) : BitArray{::vectorPiSingle[pi]} {}

    static VectorPiMask negate(_t v) { return VectorPiMask{ _mm_cmpeq_epi8(v, ::vectorOfAll[0]) }; }

    operator PieceSet() const {
        assert (isOk());
        return PieceSet( static_cast<PieceSet::_t>(_mm_movemask_epi8(this->_v)) );
    }

    bool has(Pi pi) const { return PieceSet{*this}.has(pi); }
    bool none() const { return PieceSet{*this}.none(); }
    bool isSingleton() const { return PieceSet{*this}.isSingleton(); }

    Pi index() const { return PieceSet{*this}.index(); }
    Pi smallestOne() const { return PieceSet{*this}.smallestOne(); }
    Pi largestOne() const { return PieceSet{*this}.largestOne(); }
    Pi seekVacant() const { return PieceSet{*this}.seekVacant(); }

    index_t count() const { return PieceSet{*this}.count(); }

    PieceSet begin() const { return *this; }
    PieceSet end() const { return {}; }

    friend io::ostream& operator << (io::ostream& out, VectorPiMask v) {
        return out << PieceSet{v};
    }

};

#endif
