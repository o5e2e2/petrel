#ifndef PI_MASK_HPP
#define PI_MASK_HPP

#include "io.hpp"
#include "typedefs.hpp"

#include "PieceSet.hpp"
#include "VectorOf.hpp"
#include "VectorPiSingle.hpp"

///piece vector of boolean values: false (0) or true (0xff)
class PiMask {
public:
    typedef __m128i _t;
private:
    _t m;

    typedef PiMask self_type;
    typedef self_type& self_ref;
    typedef const self_type& self_cref;

#ifdef NDEBUG
    void assertValid() const {}
#else
    void assertValid() const {
        auto v = _mm_cmpgt_epi8(::vectorOfAll[0], m);
        assert (_mm_movemask_epi8(_mm_cmpeq_epi8(v, m)) == 0xffffu);
    }
#endif

    explicit PiMask (_t a) : m{a} { assertValid(); }

public:
    constexpr operator _t () const { return m; }

    constexpr PiMask () : m{0} {}
    PiMask (Pi pi) : m{ ::vectorPiSingle[pi] } {}

    static PiMask all() { return PiMask{ ::vectorOfAll[0xff] }; }
    static PiMask cmpeq(_t a, _t b) { return PiMask { _mm_cmpeq_epi8(a, b) }; }
    static PiMask cmpgt(_t a, _t b) { return PiMask { _mm_cmpgt_epi8(a, b) }; }
    static PiMask negate(_t a) { return PiMask::cmpeq(a, ::vectorOfAll[0]); }

    operator PieceSet() const {
        assertValid();
        return PieceSet( static_cast<PieceSet::_t>(_mm_movemask_epi8(m)) );
    }

    bool has(Pi pi) const { return PieceSet(*this).has(pi); }
    bool none() const { return PieceSet(*this).none(); }
    bool isSingleton() const { return PieceSet(*this).isSingleton(); }

    Pi index() const { return PieceSet(*this).index(); }
    Pi smallestOne() const { return PieceSet(*this).smallestOne(); }
    Pi largestOne() const { return PieceSet(*this).largestOne(); }
    Pi seekVacant() const { return PieceSet(*this).seekVacant(); }

    index_t count() const { return PieceSet(*this).count(); }

    PieceSet begin() const { return *this; }
    PieceSet end() const { return {}; }

    friend io::ostream& operator << (io::ostream& out, PiMask a) {
        return out << PieceSet(a);
    }

    bool any() const { return !none(); }

    constexpr self_ref operator &= (self_cref a) { m &= a.m; return *this; }
    constexpr self_ref operator |= (self_cref a) { m |= a.m; return *this; }
    constexpr self_ref operator ^= (self_cref a) { m ^= a.m; return *this; }
    constexpr self_ref operator %= (self_cref a) { m |= a.m; m ^= a.m; return *this; }  //"and not"

    self_ref operator += (self_cref a) { assert ((*this & a).none()); m ^= a.m; return *this; }
    self_ref operator -= (self_cref a) { assert (*this >= a); m ^= a.m; return *this; }

    friend bool operator == (self_type a, self_type b) { return (a ^ b).none(); }
    friend bool operator != (self_type a, self_type b) { return !(a == b); }
    friend bool operator <= (self_type a, self_type b) { return (a & b) == a; }
    friend bool operator >= (self_type a, self_type b) { return b <= a; }

    constexpr friend self_type operator & (self_type a, self_type b) { return self_type{a} &= b; }
    constexpr friend self_type operator | (self_type a, self_type b) { return self_type{a} |= b; }
    constexpr friend self_type operator ^ (self_type a, self_type b) { return self_type{a} ^= b; }
    constexpr friend self_type operator % (self_type a, self_type b) { return self_type{a} %= b; }

    friend self_type operator + (self_type a, self_type b) { return self_type{a} += b; }
    friend self_type operator - (self_type a, self_type b) { return self_type{a} -= b; }

};

#endif
