#ifndef VECTOR_PI_MASK_HPP
#define VECTOR_PI_MASK_HPP

#include "io.hpp"
#include "Vector.hpp"

#include "Index.hpp"
#include "BitSet.hpp"
#include "VectorPiSingle.hpp"
#include "VectorOf.hpp"

/**
 * piece vector of boolean values: false (0) or true (0xff)
 */
class VectorPiMask : public BitArray<VectorPiMask, __m128i> {
    typedef BitArray<VectorPiMask, __m128i> Base;
    /**
     * class used for enumeration of piece vectors
     */
    class PieceSet : public BitSet<PieceSet, Pi, unsigned> {
    public:
        using BitSet::BitSet;

        Pi seekVacant() const {
            _t x = static_cast<_t>(*this);
            x = ~x & (x+1); //TRICK: isolate the lowest unset bit
            return PieceSet(x).index();
        }

        //for debugging
        friend std::ostream& operator << (std::ostream& out, PieceSet v) {
            auto flags(out.flags());

            out << std::hex;
            FOR_INDEX(Pi, pi) {
                if (v[pi]) {
                    out << pi;
                }
                else {
                    out << ".";
                }
            }

            out.flags(flags);
            return out;
        }

    };

    operator PieceSet() const {
        assert (isOk());
        return PieceSet( static_cast<PieceSet::_t>(_mm_movemask_epi8(static_cast<_t>(*this))) );
    }

public:
    bool isOk() const {
        _t v = _mm_cmpgt_epi8(::vectorOfAll[0], static_cast<_t>(*this));
        return _mm_movemask_epi8(_mm_cmpeq_epi8(static_cast<_t>(*this), v)) == 0xffffu;
    }

    VectorPiMask (_t v) : Base(v) { assert (isOk()); }
    VectorPiMask (Pi pi) : Base(::vectorPiSingle[pi]) {}

    bool operator[] (Pi pi) const { return PieceSet{*this}[pi]; }

    bool none() const { return PieceSet{*this}.none(); }
    bool isSingleton() const { return PieceSet{*this}.isSingleton(); }
    Pi index() const { return PieceSet{*this}.index(); }
    Pi first() const { return PieceSet{*this}.first(); }
    Pi last() const { return PieceSet{*this}.last(); }
    Pi seekVacant() const { return PieceSet{*this}.seekVacant(); }

    PieceSet begin() const { return *this; }
    PieceSet end() const { return {}; }

    //for debugging
    friend std::ostream& operator << (std::ostream& out, VectorPiMask v) {
        out << PieceSet{v};
        return out;
    }

};

#endif
