#ifndef VECTOR_PI_ENUM_HPP
#define VECTOR_PI_ENUM_HPP

#include "PiMask.hpp"
#include "VectorOf.hpp"

template <typename _Element>
class VectorPiEnum {
public:
    typedef __m128i _t;

private:
    typedef VectorPiEnum Self;
    typedef _Element element_type;

    typedef unsigned char storage_type;

    union {
        _t _v;
        storage_type byte[Pi::Size];
    };

    enum { None = 0xff };

    void _set(Pi pi, storage_type v) { byte[pi] = v; }

public:
    constexpr VectorPiEnum () : _v(::vectorOfAll[None]) {}
    constexpr explicit VectorPiEnum (_t v) : _v(v) {}
    constexpr explicit VectorPiEnum (element_type e) : _v(::vectorOfAll[e]) {}
    constexpr explicit operator _t () const { return _v; }

    PiMask notEmpty() const {
        return _mm_cmpgt_epi8(_v, ::vectorOfAll[None]);
    }

    bool isEmpty(Pi pi) const {
        return byte[pi] == static_cast<storage_type>(None);
    }

    element_type operator[] (Pi pi) const {
        assert (!isEmpty(pi));
        return static_cast<element_type>(byte[pi]);
    }

    element_type get(Pi pi) const {
        return operator[](pi);
    }

    void clear() {
        *this = {};
    }

    void drop(Pi pi, element_type e) {
        assert (isEmpty(pi));
        _set(pi, e);
    }

    void set(Pi pi, element_type e) {
        assert (!isEmpty(pi));
        _set(pi, e);
    }

    void clear(Pi pi) {
        assert (!isEmpty(pi));
        _set(pi, None);
    }

    friend PiMask operator == (Self v1, Self v2) {
        return _mm_cmpeq_epi8(v1._v, v2._v);
    }

    friend PiMask operator == (Self v, element_type e) {
        return v == VectorPiEnum{e};
    }

    friend PiMask operator > (Self v, element_type e) {
        return _mm_cmpgt_epi8(v._v, ::vectorOfAll[e]);
    }

    friend PiMask operator < (Self v, element_type e) {
        return _mm_cmpgt_epi8(::vectorOfAll[e], v._v);
    }

};

#endif
