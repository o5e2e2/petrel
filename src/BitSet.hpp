#ifndef BIT_SET_HPP
#define BIT_SET_HPP

#include "bitops.hpp"
#include "BitArray.hpp"

template <class Self, class Index, class Value = int>
class BitSet : public BitArray<Self, Value> {
    typedef BitArray<Self, Value> Base;

protected:
    using Base::self;

public:
    using typename Base::_t;

    constexpr BitSet () : Base() {}
    constexpr explicit BitSet (_t v) : Base(v) {}
    constexpr BitSet (Index i) : BitSet(::singleton<_t>(i)) {}
    constexpr BitSet (typename Index::_t i) : BitSet(::singleton<_t>(i)) {}

    constexpr _t without_lsb() const { return ::without_lsb(static_cast<_t>(*this)); }

    constexpr bool operator [] (Index i) const { return (self() & Self{i}).any(); }
    bool isSingleton() const { assert (self().any()); return without_lsb() == 0; }

    constexpr Index first() const { return static_cast<typename Index::_t>(::bsf(static_cast<_t>(*this))); }
    constexpr Index last()  const { return static_cast<typename Index::_t>(::bsr(static_cast<_t>(*this))); }

    Index index() const { assert (self().any() && self().isSingleton()); return self().first(); }

    constexpr Index operator * () const { return self().first(); }
    Self& operator ++ () { *this = Self(without_lsb()); return self(); }

    constexpr Self begin() const { return self(); }
    constexpr Self end() const { return self().empty(); }

    friend bool operator >> (Self& self, Index& i) {
        if (self.none()) { return false; }
        i = *self;
        ++self;
        return true;
    }

    friend bool operator << (Index& i, Self& self) {
        if (self.none()) { return false; }
        i = self.last();
        self -= i;
        return true;
    }

};

#endif
