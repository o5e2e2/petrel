#ifndef BIT_SET_HPP
#define BIT_SET_HPP

#include "bitops.hpp"
#include "BitArray.hpp"

template <class Self, class Index, class Value = unsigned int>
class BitSet : public BitArray<Self, Value> {
    typedef BitArray<Self, Value> Base;

protected:
    using Base::self;

public:
    using typename Base::_t;

    constexpr BitSet () : Base{} {}
    constexpr explicit BitSet (_t v) : Base{v} {}

    constexpr BitSet (Index i) : BitSet{::singleton<_t>(i)} {}
    constexpr BitSet (typename Index::_t i) : BitSet{::singleton<_t>(i)} {}

    constexpr _t withoutLsb() const { return ::withoutLsb(this->_v); }

    constexpr bool has(Index i) const { return (self() & Self{i}).any(); }
    bool isSingleton() const { assert (self().any()); return withoutLsb() == 0; }

    constexpr Index smallestOne() const { return static_cast<typename Index::_t>(::bsf(this->_v)); }
    constexpr Index largestOne()  const { return static_cast<typename Index::_t>(::bsr(this->_v)); }

    Index index() const { assert (self().any() && self().isSingleton()); return *self(); }

    constexpr Index operator * () const { return self().smallestOne(); }
    Self& operator ++ () { *this = Self(withoutLsb()); return self(); }

    constexpr Self begin() const { return self(); }
    constexpr Self end() const { return Self{}; }

};

#endif
