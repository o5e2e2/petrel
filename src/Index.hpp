#ifndef INDEX_HPP
#define INDEX_HPP

#include <array>
#include <cstring>
#include "types.hpp"
#include "bitops.hpp"
#include "io.hpp"

#define FOR_EACH(Index, i) for (auto i : Index( static_cast<Index::_t>(0) ))

template <size_t _Size, typename _Value_type = index_t, class _Actual_type = _Value_type>
class Index {
    static io::czstring The_string;

    typedef index_t _Storage_type;

public:
    typedef _Value_type _t;

    enum { Size = _Size, Mask = Size-1 };
    static const _t Begin = static_cast<_t>(0);

    template <typename T> using arrayOf = std::array<T, Size>;

protected:
    _Storage_type v;

public:
    constexpr Index () : v{Size} {}
    constexpr Index (_t i) : v{i} { assertValid(); }

    constexpr operator _t () const { return static_cast<_t>(v); }

    constexpr void assertValid() const { assert (isValid()); }
    constexpr bool isValid() const { return static_cast<_Storage_type>(v) < static_cast<_Storage_type>(Size); }

    constexpr bool is(_t i) const { return v == i; }

    constexpr _Actual_type operator * () const { return static_cast<_t>(v); }
    constexpr static Index begin () { return static_cast<_t>(0); }
    constexpr static Index end()    { return Index(); }

    constexpr Index& operator ++ () {
        assertValid();
        ++v;
        return *this;
    }

    constexpr Index& operator -- () {
        --v;
        assertValid();
        return *this;
    }

    constexpr Index& flip() {
        assertValid();
        v ^= Mask;
        return *this;
    }
    constexpr Index operator ~ () const { return Index{*this}.flip(); }

    io::char_type to_char() const { return The_string[*this]; }
    friend io::ostream& operator << (io::ostream& out, Index i) { return out << i.to_char(); }

    bool from_char(io::char_type c) {
        auto p = std::memchr(The_string, c, Size);
        if (!p) { return false; }
        this->v = static_cast<_t>(static_cast<io::czstring>(p) - The_string);
        assert (c == to_char());
        return true;
    }

    friend io::istream& read(io::istream& in, Index& i) {
        io::char_type c;
        if (in.get(c)) {
            if (!i.from_char(c)) { io::fail_char(in); }
        }
        return in;
    }

};

#endif
