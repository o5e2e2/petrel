#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "typedefs.hpp"

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
    enum { RankShift = 3, RankOffset = (1 << RankShift), RankMask = (Rank::Mask << RankShift) };

    using Index::Index;
    constexpr Square (File f, Rank r) : Index{static_cast<_t>(f + (r << RankShift))} {}

    constexpr explicit operator File() const { return static_cast<File::_t>(this->_v & File::Mask); }
    constexpr explicit operator Rank() const { return static_cast<Rank::_t>(static_cast<unsigned>(this->_v) >> RankShift); }

    Square& flip() { this->_v = static_cast<_t>(this->_v ^ RankMask); return *this; }
    constexpr Square operator ~ () const { return static_cast<_t>(this->_v ^ RankMask); }
    constexpr Square rankUp() const { return static_cast<_t>(this->_v - RankOffset); }

    using Index::is;
    constexpr bool is(Rank rank) const { return (this->_v & RankMask) == (rank << RankShift); }
    constexpr bool is(File file) const { return (this->_v & File::Mask) == file; }

    //PieceTypeAttack table initialization
    constexpr signed x88(signed d_file, signed d_rank) const;
    constexpr Bb operator() (signed d_file, signed d_rank) const;

    friend std::ostream& operator << (std::ostream& out, Square sq) {
        return out << File{sq} << Rank{sq};
    }

    friend std::istream& operator >> (std::istream& in, Square& sq) {
        auto before = in.tellg();
        File file{File::Begin}; Rank rank{Rank::Begin};

        if (in >> file >> rank) {
            sq = Square{file, rank};
        }
        else {
            io::fail_pos(in, before);
        }
        return in;
    }

};

constexpr CastlingSide castlingSide(Square king, Square rook) { return (rook < king)? QueenSide : KingSide; }

#endif
