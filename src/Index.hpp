#ifndef INDEX_HPP
#define INDEX_HPP

#include <array>
#include <cstring>
#include "types.hpp"
#include "bitops.hpp"
#include "io.hpp"

#define FOR_INDEX(Index, i) for (Index i{Index::Begin}; i.isOk(); ++i)

template <size_t _Size, typename _Value_type = index_t, typename _Storage_type = index_t>
class Index {
    static io::czstring The_string;
public:
    typedef _Value_type _t;

    enum { Size = _Size, Mask = Size-1 };
    static const _t Begin = static_cast<_t>(0);

protected:
    _Storage_type _v;

public:
    constexpr Index () : _v{Size} {}
    constexpr Index (_t v) : _v{v} { assertValid(); }

    constexpr operator _t () const { return static_cast<_t>(_v); }

    constexpr void assertValid() const { assert (isOk()); }
    constexpr bool isOk() const { return static_cast<_Storage_type>(_v) < static_cast<_Storage_type>(Size); }

    constexpr bool is(_t v) const { return _v == v; }

    constexpr Index& operator ++ () {
        assertValid();
        ++_v;
        return *this;
    }

    constexpr Index& operator -- () {
        --_v;
        assertValid();
        return *this;
    }

    constexpr Index& flip() {
        assertValid();
        _v ^= Mask;
        return *this;
    }
    constexpr Index operator ~ () const { return Index{*this}.flip(); }

    bool from_char(io::char_type c) {
        if (const void* p = std::memchr(The_string, c, Size)) {
            this->_v = static_cast<_t>(static_cast<io::czstring>(p) - The_string);
            assert (c == to_char());
            return true;
        }
        return false;
    }
    constexpr const io::char_type& to_char() const { return The_string[*this]; }

    friend io::istream& operator >> (io::istream& in, Index& i) {
        io::char_type c;
        if (in.get(c)) {
            if (!i.from_char(c)) { io::fail_char(in); }
        }
        return in;
    }

    friend io::ostream& operator << (io::ostream& out, Index i) {
        return out << i.to_char();
    }

    template <typename T> using arrayOf = std::array<T, Size>;

};

#endif
