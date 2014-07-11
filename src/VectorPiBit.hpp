#ifndef VECTOR_PI_BIT_HPP
#define VECTOR_PI_BIT_HPP

#include "Vector.hpp"
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

    template <index_type _Bit>
    static element_type single() { return ::singleton<element_type>(_Bit); }

    template <index_type _Bit>
    static element_type exceptSingle() { return static_cast<element_type>(0xff ^ single<_Bit>()); }

    template <index_type _Bit>
    static _t singleVector() { return ::vectorOfAll[single<_Bit>()]; }

    template <index_type _Bit>
    static _t exceptSingleVector() { return ::vectorOfAll[exceptSingle<_Bit>()]; }

public:
    using Base::Base;

    bool isEmpty(Pi pi) const { return (*this)[pi] == 0; }
    void clear() { *this = Self(); }
    void clear(Pi pi) { (*this)[pi] = 0; }
    void set(Pi pi, element_type v) { (*this)[pi] = v; }
    void set(Pi pi, index_type i) { set(pi, ::singleton<element_type>(i)); }

    const element_type& operator [] (Pi pi) const { return reinterpret_cast<const element_type*>(this)[pi]; }
    element_type& operator [] (Pi pi) { return reinterpret_cast<element_type*>(this)[pi]; }

    template <index_type _Bit>
    bool is(Pi pi) const { return ((*this)[pi] & single<_Bit>()) != 0; }

    template <index_type _Bit>
    VectorPiMask get() const {
        _t mask = singleVector<_Bit>();
        return _mm_cmpeq_epi8(mask, static_cast<_t>(*this) & mask);
    }

    VectorPiMask noneMask() const {
        return _mm_cmpeq_epi8(static_cast<_t>(*this), ::vectorOfAll[0]);
    }

    VectorPiMask notEmpty() const {
        return _mm_cmpeq_epi8(_mm_cmpeq_epi8(static_cast<_t>(*this), ::vectorOfAll[0]), ::vectorOfAll[0]);
    }

    template <element_type bitmask>
    VectorPiMask getAnyMask() const {
        _t mask = ::vectorOfAll[bitmask];
        return _mm_cmpeq_epi8(_mm_cmpeq_epi8(static_cast<_t>(*this) & mask, ::vectorOfAll[0]), ::vectorOfAll[0]);
    }

    template <element_type bitmask>
    VectorPiMask getAllMask() const {
        _t mask = ::vectorOfAll[bitmask];
        return _mm_cmpeq_epi8(mask, static_cast<_t>(*this) & mask);
    }

    template <index_type _Bit>
    void clear() {
        this->_v &= exceptSingleVector<_Bit>();
    }

    template <index_type _Bit, index_type bitmask>
    void clearMasked() {
        _t mask = singleVector<bitmask>();
        this->_v &= _mm_cmpeq_epi8(static_cast<_t>(*this) & mask, ::vectorOfAll[0]) | exceptSingleVector<_Bit>();
    }

    template <index_type _Bit>
    void clear(Pi pi) {
        element_type mask{exceptSingle<_Bit>()};
        (*this)[pi] &= mask;
    }

    template <index_type _Bit>
    void set(Pi pi) {
        element_type mask{single<_Bit>()};
        (*this)[pi] |= mask;
    }

};

#endif
