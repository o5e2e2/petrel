#ifndef BIT_ARRAY_HPP
#define BIT_ARRAY_HPP

template <typename _ValueType>
class BitArrayBase {
    typedef BitArrayBase B;

protected:
    typedef _ValueType _t;
    _t _v;

    constexpr BitArrayBase () : _v{} {}
    constexpr explicit BitArrayBase (_t v) : _v(v) {}

    constexpr void operator &= (B b) { _v &= b._v; }
    constexpr void operator |= (B b) { _v |= b._v; }
    constexpr void operator ^= (B b) { _v ^= b._v; }
    constexpr void operator %= (B b) { _v |= b._v; _v ^= b._v; }  //"and not"
    bool operator == (B b) { return _v == b._v; }

public:
    constexpr explicit operator _t () const { return _v; }
};

/*
 * typesafe bit array implementation using so called "curiously recurring template pattern"
 */
template <class _Self, typename _ValueType>
class BitArray : protected BitArrayBase<_ValueType> {
public:
    using Base = BitArrayBase<_ValueType>;
    typedef _Self Self;
    typedef Self Arg;

protected:
    constexpr Self& self() { return static_cast<Self&>(*this); }
    constexpr const Self& self() const { return static_cast<const Self&>(*this); }

    using Base::Base;

public:
    using typename Base::_t;
    using Base::operator _t;

    constexpr bool none() const { return self() == Self{}; }
    constexpr bool any()  const { return !self().none(); }

    constexpr bool none(Arg a) const { return (self() & a).none(); }

    constexpr Self& operator  = (Arg a) { Base::operator  = (a); return self(); }
    constexpr Self& operator &= (Arg a) { Base::operator &= (a); return self(); }
    constexpr Self& operator |= (Arg a) { Base::operator |= (a); return self(); }
    constexpr Self& operator ^= (Arg a) { Base::operator ^= (a); return self(); }
    constexpr Self& operator %= (Arg a) { Base::operator %= (a); return self(); }  //"and not"
    constexpr Self& operator += (Arg a) { assert (none(a)); return self() ^= a; }
    constexpr Self& operator -= (Arg a) { assert (self() >= a); return self() ^= a; }

    constexpr friend bool operator == (Arg a, Arg b) { return a.operator== (b); }
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
