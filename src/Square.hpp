#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "Index.hpp"

enum file_t { FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH };
typedef Index<8, file_t> File;

enum rank_t { Rank8, Rank7, Rank6, Rank5, Rank4, Rank3, Rank2, Rank1 };
typedef Index<8, rank_t> Rank;

inline constexpr Rank up(Rank r) { return static_cast<Rank::_t>(r-1); }

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
    typedef Index<64, _t> Base;

    enum { RankMask = 070 };

    using Base::Base;
    constexpr Square (File f, Rank r) : Base{static_cast<_t>(f + (r << 3))} {}
    constexpr Square (Square sq, PromoType promo) : Square(File{sq}, static_cast<Rank::_t>(+promo)) {}  //encoding of underpromotions

    constexpr explicit operator File() const { return static_cast<File::_t>(static_cast<unsigned>(*this) & File::Mask); }
    constexpr explicit operator Rank() const { return static_cast<Rank::_t>(*this >> 3); }
    constexpr explicit operator PromoType() const { return static_cast<PromoType::_t>(+Rank{*this}); } //encoding of underpromotions

    Square& flip() { *this = static_cast<_t>(*this ^ RankMask); return *this; }
    constexpr Square operator ~ () const { return static_cast<_t>(*this ^ RankMask); }
    constexpr Square rankUp() const { return static_cast<_t>(*this - 010); }
    constexpr Square rankDown() const { return static_cast<_t>(*this + 010); }

    template <Rank::_t Rank> constexpr bool is() const { return (*this & RankMask) == (Rank << 3); }
    template <File::_t File> constexpr bool is() const { return (*this & static_cast<File::_t>(File::Mask)) == File; }

    constexpr signed x88(signed fileOffset, signed rankOffset) const {
        return static_cast<_t>(*this) + (static_cast<_t>(*this) & 070) + fileOffset + 16*rankOffset;
    }

    constexpr Bb operator() (signed dFile, signed dRank) const;

    friend std::ostream& operator << (std::ostream& out, Square sq) { return out << File{sq} << Rank{sq}; }

    friend std::istream& operator >> (std::istream& in, Square& sq) {
        File file{File::Begin}; Rank rank{Rank::Begin};
        if (in >> file >> rank) { sq = Square{file, rank}; }
        return in;
    }
};

#endif
