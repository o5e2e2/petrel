#ifndef INDEX_HPP
#define INDEX_HPP

#include <array>
#include <cstring>
#include "bitops.hpp"
#include "io.hpp"

#define FOR_INDEX(Index, i) for (Index i{Index::Begin}; i.isOk(); ++i)

template <int _Limit, typename _Value = int>
class Index {
public:
    typedef _Value _t;
    typedef std::istream::char_type char_type;

    enum { Mask = _Limit-1, Size = _Limit };
    static const _t Begin = static_cast<_t>(0);

private:
    int _v;
    static const char_type* The_string;

public:
    constexpr Index () : _v{Size} {}
    constexpr Index (_t v) : _v{v} {}
    constexpr operator _t () const { return static_cast<_t>(_v); }

    constexpr void assertValid() const { assert (isOk()); }
    constexpr bool isOk() const { return static_cast<unsigned>(_v) < static_cast<unsigned>(Size); }

    Index& operator ++ () {
        assertValid();
        ++_v;
        return *this;
    }

    Index& flip() {
        assertValid();
        _v ^= Mask;
        return *this;
    }

    constexpr Index operator ~ () const { return Index{*this}.flip(); }
    constexpr const char_type& to_char() const { return The_string[*this]; }

    bool from_char(char_type c) {
        if (const void* p = std::memchr(The_string, c, _Limit)) {
            _v = static_cast<_t>(static_cast<const char_type*>(p) - The_string);
            assert (c == to_char());
            return true;
        }
        return false;
    }

    friend std::istream& operator >> (std::istream& in, Index& i) {
        char_type c;
        if (in.get(c)) {
            if (!i.from_char(c)) { ::fail_char(in); }
        }
        return in;
    }

    friend std::ostream& operator << (std::ostream& out, Index i) {
        return out << i.to_char();
    }

    template <typename T> struct array : std::array<T, Size> {
        typedef Index index_type;

        typedef std::array<T, Size> Base;
        using typename Base::size_type;
        using Base::Base;

#if defined ASSERT_INDEX_CHECK
        using std::array<T, Size>::at;
        T& operator[] (Index i) { return at(static_cast<size_type>(i)); };
        const T& operator[] (Index i) const { return at(static_cast<size_type>(i)); };
#else
        using std::array<T, Size>::operator[];
        T& operator[] (Index i) { return operator[](static_cast<size_type>(i)); };
        const T& operator[] (Index i) const { return operator[](static_cast<size_type>(i)); };
#endif
    };

    template <typename T> struct CACHE_ALIGN static_array : array<T> {
        static_array () = default;
        static_array (const static_array&) = delete;
        static_array& operator = (const static_array&) = delete;
    };

};

enum color_t { White, Black };
typedef Index<2, color_t> Color;

enum side_t { My, Op };
typedef Index<2, side_t> Side;

enum castling_side_t { KingSide, QueenSide };
typedef Index<2, castling_side_t> CastlingSide;

enum piece_index_t { TheKing };
typedef Index<16, piece_index_t> Pi; //piece index

enum piece_type_t { Queen, Rook, Bishop, Knight, Pawn, King };
typedef Index<3, piece_type_t> SliderType;
typedef Index<4, piece_type_t> PromoType;
typedef Index<6, piece_type_t> PieceType;

enum chess_variant_t { Orthodox, Chess960 };
typedef Index<2, chess_variant_t> ChessVariant;

#endif
