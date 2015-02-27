#ifndef BB_HPP
#define BB_HPP

#include "io.hpp"
#include "bitops.hpp"
#include "BitSet.hpp"
#include "BitRank.hpp"
#include "Square.hpp"

#if defined _WIN32
#   define BB(number) number##ull
#else
#   define BB(number) number##ul
#endif

/**
 * BitBoard type: a bit for each chessboard square
 */
class Bb : public BitSet<Bb, Square, std::uint64_t> {
    typedef BitSet<Bb, Square, std::uint64_t> Base;

    Bb (int) = delete; //declared to catch type cast bugs

    //bidirectional signed shift
    constexpr Bb (_t v, signed offset) : Bb( (offset >= 0)? (v << offset):(v >> -offset) ) {}

public:
    constexpr Bb () : Base() {}
    constexpr Bb (Square sq) : Base(sq) {}
    constexpr Bb (Square::_t sq) : Base(sq) {}
    constexpr explicit Bb (_t v) : Base(v) {}

    constexpr explicit Bb (Rank::_t r) : Bb{BB(0xff) << 8*r} {}
    constexpr explicit Bb (File::_t f) : Bb{BB(0x0101010101010101) << f} {}

    Bb operator ~ () const { return Bb{::bswap(this->_v)}; }

    void move(Square from, Square to) { assert (from != to); *this -= from; *this += to; }

    using BitSet::operator[];
    constexpr BitRank operator[] (Rank r) const { return BitRank(small_cast<BitRank::_t>(this->_v >> 8*r)); }

    constexpr friend Bb operator << (Bb b, unsigned offset) { return Bb{static_cast<_t>(b) << offset}; }
    constexpr friend Bb operator >> (Bb b, unsigned offset) { return Bb{static_cast<_t>(b) >> offset}; }

    constexpr static Bb empty() { return {}; }
    constexpr static Bb horizont(Square sq) { return Bb{Rank(sq)} - sq; }
    constexpr static Bb vertical(Square sq) { return Bb{File(sq)} - sq; }
    constexpr static Bb diagonal(Square sq) { return Bb{BB(0x0102040810204080), 8*(Rank(sq) + File(sq) - 7)} - sq; }
    constexpr static Bb antidiag(Square sq) { return Bb{BB(0x8040201008040201), 8*(Rank(sq) - File(sq))} - sq; }

    constexpr explicit operator __int64 () const { return static_cast<__int64>(this->_v); }

    friend std::ostream& operator << (std::ostream& out, Bb bb) {
        FOR_INDEX(Rank, rank) {
            FOR_INDEX(File, file) {
                out << (bb[Square(file, rank)]? file:'.');
            }
            out << '\n';
        }
        return out;
    }

};

#endif
