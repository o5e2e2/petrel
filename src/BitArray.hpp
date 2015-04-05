#ifndef BIT_ARRAY_HPP
#define BIT_ARRAY_HPP

template <typename _ValueType>
class BitArrayBase {
    typedef BitArrayBase Self;
    typedef BitArrayBase Arg;

protected:
    typedef _ValueType _t;

    constexpr BitArrayBase () : _v(0) {}
    constexpr explicit BitArrayBase (_t v) : _v(v) {}
    constexpr explicit operator _t () const { return _v; }

    Self& operator = (Arg a) { _v  = a._v; return *this; }

    void operator &= (Arg a) { _v &= a._v; }
    void operator |= (Arg a) { _v |= a._v; }
    void operator ^= (Arg a) { _v ^= a._v; }
    void operator %= (Arg a) { _v |= a._v; _v ^= a._v; }  //"and not"

    bool operator == (Arg a) { return _v == a._v; }

protected:
    _t _v;
};

/*
 * typesafe bit array implementation using so called "curiously recurring template pattern"
 */
template <class _Self, typename _ValueType>
class BitArray : protected BitArrayBase<_ValueType> {
    typedef BitArrayBase<_ValueType> Base;

public:
    typedef _Self Self;
    typedef Self Arg;

protected:
    Self& self() { return static_cast<Self&>(*this); }
    const Self& self() const { return static_cast<const Self&>(*this); }

    using Base::Base;

public:
    using typename Base::_t;
    using Base::operator _t;

    constexpr bool none() const { return self() == Self{}; }
    constexpr bool any()  const { return !self().none(); }

    constexpr bool none(Arg a) const { return (self() & a).none(); }

    Self& operator  = (Arg a) { Base::operator = (a); return self(); }
    Self& operator &= (Arg a) { Base::operator &= (a); return self(); }
    Self& operator |= (Arg a) { Base::operator |= (a); return self(); }
    Self& operator ^= (Arg a) { Base::operator ^= (a); return self(); }
    Self& operator %= (Arg a) { Base::operator %= (a); return self(); }  //"and not"
    Self& operator += (Arg a) { assert (none(a)); return self() ^= a; }
    Self& operator -= (Arg a) { assert (self() >= a); return self() ^= a; }

    constexpr friend bool operator == (Arg a, Arg b) { return a.Base::operator== (b); }
    constexpr friend bool operator != (Arg a, Arg b) { return !(a == b); }
    constexpr friend bool operator <= (Arg a, Arg b) { return (a & b) == a; }
    constexpr friend bool operator >= (Arg a, Arg b) { return b <= a; }
    constexpr friend Self operator &  (Arg a, Arg b) { return Self{a} &= b; }
    constexpr friend Self operator |  (Arg a, Arg b) { return Self{a} |= b; }
    constexpr friend Self operator ^  (Arg a, Arg b) { return Self{a} ^= b; }
    constexpr friend Self operator %  (Arg a, Arg b) { return Self{a} %= b; }
    constexpr friend Self operator +  (Arg a, Arg b) { return Self{a} += b; }
    constexpr friend Self operator -  (Arg a, Arg b) { return Self{a} -= b; }

};

#endif
