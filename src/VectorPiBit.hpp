#ifndef VECTOR_PI_BIT_HPP
#define VECTOR_PI_BIT_HPP

#include "BitArray128.hpp"
#include "VectorOf.hpp"
#include "VectorPiMask.hpp"

template <class Self, class Index>
class VectorPiBit : public BitArray<Self, __m128i> {
    typedef BitArray<Self, __m128i> Base;
public:
    typedef typename Base::_t _t;
    typedef std::uint8_t element_type;
protected:
    typedef typename Index::_t index_type;

    constexpr static element_type single(index_type _Bit) { return ::singleton<element_type>(_Bit); }
    constexpr static element_type exceptSingle(index_type _Bit) { return static_cast<element_type>(0xff ^ single(_Bit)); }

    static _t singleVector(index_type _Bit) { return ::vectorOfAll[single(_Bit)]; }
    static _t exceptSingleVector(index_type _Bit) { return ::vectorOfAll[exceptSingle(_Bit)]; }

public:
    using Base::Base;

    constexpr bool isEmpty(Pi pi) const { return (*this)[pi] == 0; }
    void clear() { *this = Self(); }
    void clear(Pi pi) { (*this)[pi] = 0; }

    constexpr const element_type& operator [] (Pi pi) const { return reinterpret_cast<const element_type*>(this)[pi]; }
    element_type& operator [] (Pi pi) { return reinterpret_cast<element_type*>(this)[pi]; }

    constexpr bool is(Pi pi, element_type bitmask) const { return ((*this)[pi] & bitmask) != 0; }
    constexpr bool is(Pi pi, index_type _Bit) const { return is(pi, single(_Bit)); }

    VectorPiMask anyOf(index_type _Bit) const {
        return allOf(single(_Bit));
    }

    VectorPiMask allOf(element_type bitmask) const {
        _t mask = ::vectorOfAll[bitmask];
        return _mm_cmpeq_epi8(mask, this->_v & mask);
    }

    VectorPiMask anyOf(element_type bitmask) const {
        _t mask = ::vectorOfAll[bitmask];
        return VectorPiMask::negate(_mm_cmpeq_epi8(this->_v & mask, ::vectorOfAll[0]));
    }

    VectorPiMask notEmpty() const {
        return VectorPiMask::negate(_mm_cmpeq_epi8(this->_v, ::vectorOfAll[0]));
    }

    void clear(index_type _Bit) {
        this->_v &= exceptSingleVector(_Bit);
    }

    void clear(Pi pi, index_type _Bit) {
        element_type mask{exceptSingle(_Bit)};
        (*this)[pi] &= mask;
    }

    void set(Pi pi, index_type _Bit) {
        element_type mask{single(_Bit)};
        (*this)[pi] |= mask;
    }

};

#endif
