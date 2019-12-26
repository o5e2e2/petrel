#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "io.hpp"
#include "Index.hpp"

enum square_t {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1
};

class Bb;
struct Square : Index<64, square_t> {
    enum { RankShift = 3, RankMask = (Rank::Mask << RankShift) };

    using Index::Index;
    constexpr Square () = default;
    constexpr Square (File file, Rank rank) : Index{static_cast<_t>(file + (rank << RankShift))} {}

    constexpr explicit operator File() const { return static_cast<File::_t>(this->_v & File::Mask); }
    constexpr explicit operator Rank() const { return static_cast<Rank::_t>(static_cast<unsigned>(this->_v) >> RankShift); }
    constexpr explicit operator PromoType() const { return static_cast<PromoType::_t>(+Rank(*this)); }

    constexpr Square& flip() { this->_v = static_cast<_t>(this->_v ^ RankMask); return *this; }
    constexpr Square operator ~ () const { return static_cast<_t>(this->_v ^ RankMask); }
    constexpr Square rankForward() const { return static_cast<_t>(this->_v - A7 + A8); }

    constexpr bool on(Rank rank) const { return (this->_v & RankMask) == static_cast<unsigned>(rank << RankShift); }
    constexpr bool on(File file) const { return (this->_v & File::Mask) == file; }

    constexpr Bb horizont() const;
    constexpr Bb vertical() const;
    constexpr Bb diagonal() const;
    constexpr Bb antidiag() const;
    constexpr Bb operator() (signed fileOffset, signed rankOffset) const;

    constexpr signed x88(signed fileOffset, signed rankOffset) const {
        return static_cast<signed>(this->_v + (this->_v & 070)) + fileOffset + 16*rankOffset;
    }

    friend io::ostream& operator << (io::ostream& out, Square sq) {
        return out << File(sq) << Rank(sq);
    }

    friend io::istream& operator >> (io::istream& in, Square& sq) {
        auto here = in.tellg();

        File file{File::Begin};
        Rank rank{Rank::Begin};
        in >> file >> rank;

        if (in) {
            sq = Square{file, rank};
        }
        else {
            io::fail_pos(in, here);
        }
        return in;
    }

};

#endif
