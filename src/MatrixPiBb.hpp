#ifndef MATRIX_PI_BB_H
#define MATRIX_PI_BB_H

#include "Bb.hpp"
#include "VectorOf.hpp"
#include "VectorPiRank.hpp"
#include "ReverseBb.hpp"

class MatrixPiBb {
    Rank::arrayOf<VectorPiRank> matrix;

public:
    constexpr MatrixPiBb () = default;

    friend bool operator == (const MatrixPiBb& a, const MatrixPiBb& b) {
        FOR_INDEX(Rank, rank) {
            if (a.matrix[rank] != b.matrix[rank]) { return false; }
        }
        return true;
    }

    friend void swap(MatrixPiBb& a, MatrixPiBb& b) {
        FOR_INDEX(Rank, rank) {
            using std::swap;
            swap(a.matrix[rank], b.matrix[rank]);
        }
    }

    void clear() {
        FOR_INDEX(Rank, rank) {
            matrix[rank].clear();
        }
    }

    void clear(Pi pi, Square sq) {
        matrix[Rank(sq)] -= VectorPiRank{File(sq)} & PiMask{pi};
    }

    void clear(Pi pi) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] %= PiMask{pi};
        }
    }

    void set(Pi pi, Rank rank, BitRank br) {
        //_mm_blendv_epi8
        matrix[rank] = (matrix[rank] % PiMask{pi}) + (VectorPiRank{br} & PiMask{pi});
    }

    void set(Pi pi, Bb bb) {
        FOR_INDEX(Rank, rank) {
            set(pi, rank, bb[rank]);
        }
    }

    void add(Pi pi, Rank rank, File file) {
        matrix[rank] += VectorPiRank{file} & PiMask{pi};
    }

    void add(Pi pi, Square sq) {
        add(pi, Rank(sq), File(sq));
    }

    void add(Pi pi, Bb bb) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] += VectorPiRank{bb[rank]} & PiMask{pi};
        }
    }

    bool has(Pi pi, Square sq) const {
        return (matrix[Rank(sq)] & VectorPiRank{File(sq)} & PiMask{pi}).any();
    }

    const VectorPiRank& operator[] (Rank rank) const {
        return matrix[rank];
    }

    VectorPiRank& operator[] (Rank rank) {
        return matrix[rank];
    }

    //pieces affecting the given square
    PiMask operator[] (Square sq) const {
        return matrix[Rank(sq)][File(sq)];
    }

    //bitboard of the given piece
    Bb operator[] (Pi pi) const {
        Bb::_t b{0};
        FOR_INDEX(Rank, rank) {
            b += static_cast<Bb::_t>(static_cast<BitRank::_t>((matrix[rank][pi]))) << 8*rank;
        }
        return Bb{b};
    }

    //bitboard of squares affected by all pieces
    Bb gather() const {
        Bb::_t b{0};
        FOR_INDEX(Rank, rank) {
            b += static_cast<Bb::_t>(static_cast<BitRank::_t>(matrix[rank].gather())) << 8*rank;
        }
        return Bb{b};
    }

    void filter(Pi pi, Bb bb) {
        PiMask exceptPi{ ::vectorOfAll[0xff] ^ ::vectorPiSingle[pi] };
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= VectorPiRank{bb[rank]} | exceptPi;
        }
    }

    friend MatrixPiBb operator % (const MatrixPiBb& from, Bb bb) {
        MatrixPiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] % VectorPiRank{bb[rank]};
        }
        return result;
    }

    void operator &= (Bb bb) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= VectorPiRank{bb[rank]};
        }
    }

    friend MatrixPiBb operator & (const MatrixPiBb& from, Bb bb) {
        MatrixPiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] & VectorPiRank{bb[rank]};
        }
        return result;
    }

    index_t count() const {
        VectorBitCount::_t result = ::vectorOfAll[0];
        FOR_INDEX(Rank, rank) {
            result = _mm_add_epi8(result, ::bitCount.perByte(static_cast<VectorBitCount::_t>(matrix[rank])));
        }
        return ::bitCount.total(result);
    }

};

#endif
