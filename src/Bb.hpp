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
class Bb : public BitSet<Bb, Square, u64_t> {
    //declared to catch type cast bugs
    Bb (unsigned int) = delete;
    Bb (int) = delete;

public:
    constexpr Bb () : BitSet() {}
    constexpr explicit Bb (_t bb) : BitSet{bb} {}

    constexpr Bb (Square sq) : BitSet(sq) {}
    constexpr Bb (Square::_t sq) : BitSet(sq) {}

    constexpr explicit Bb (File::_t f) : Bb{BB(0x0101010101010101) << f} {}
    constexpr explicit Bb (Rank::_t r) : Bb{BB(0xff) << 8*r} {}

    //bidirectional signed shift
    constexpr Bb (_t bb, signed offset) : Bb( (offset >= 0) ? (bb << offset) : (bb >> -offset) ) {}

    constexpr explicit operator i64_t () const { return static_cast<i64_t>(this->v); }
    constexpr explicit operator u64_t () const { return static_cast<u64_t>(this->v); }

    Bb operator ~ () const { return Bb{::bswap(this->v)}; }

    void move(Square from, Square to) { assert (from != to); *this -= from; *this += to; }

    constexpr BitRank operator[] (Rank r) const { return BitRank(small_cast<BitRank::_t>(this->v >> 8*r)); }

    constexpr friend Bb operator << (Bb bb, unsigned offset) { return Bb{static_cast<_t>(bb) << offset}; }
    constexpr friend Bb operator >> (Bb bb, unsigned offset) { return Bb{static_cast<_t>(bb) >> offset}; }

    friend io::ostream& operator << (io::ostream& out, Bb bb) {
        FOR_EACH(Rank, rank) {
            FOR_EACH(File, file) {
                if (bb.has(Square{file, rank})) {
                    out << file;
                }
                else {
                    out << '.';
                }
            }
            out << '\n';
        }
        return out;
    }

};

constexpr Bb Square::operator() (signed f, signed r) const { return isValidOffset(f, r) ? Bb{offset(f, r)} : Bb{}; }
constexpr Bb Square::horizont() const { return Bb{Rank(*this)} - *this; }
constexpr Bb Square::vertical() const { return Bb{File(*this)} - *this; }
constexpr Bb Square::diagonal() const { return Bb{BB(0x0102040810204080), 8*(Rank(*this) + +File(*this) - 7)} - *this; }
constexpr Bb Square::antidiag() const { return Bb{BB(0x8040201008040201), 8*(Rank(*this) - +File(*this))} - *this; }

#endif
