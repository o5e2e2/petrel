#ifndef MATRIX_PI_BB_H
#define MATRIX_PI_BB_H

#include "Bb.hpp"
#include "VectorOf.hpp"
#include "VectorPiRank.hpp"
#include "ReverseBb.hpp"

class MatrixPiBb {
    Rank::array<VectorPiRank> matrix;

public:
    MatrixPiBb () { clear(); }
    MatrixPiBb (const MatrixPiBb& from) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] = from.matrix[rank];
        }
    }

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

    void clear(Pi pi) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] %= VectorPiMask{pi};
        }
    }

    void set(Pi pi, Rank rank, BitRank b) {
        //_mm_blendv_epi8
        matrix[rank] = (matrix[rank] % VectorPiMask{pi}) + (VectorPiRank{b} & VectorPiMask{pi});
    }

    void set(Pi pi, Bb b) {
        FOR_INDEX(Rank, rank) {
            set(pi, rank, b[rank]);
        }
    }

    void add(Pi pi, Bb b) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] += VectorPiRank{b[rank]} & VectorPiMask{pi};
        }
    }

    bool is(Pi pi, Square sq) const {
        return (matrix[Rank{sq}] & VectorPiMask{pi} & VectorPiRank{File{sq}}).any();
    }

    void add(Pi pi, Square sq) {
        add(pi, File{sq}, Rank{sq});
    }

    void add(Pi pi, File file, Rank rank) {
        matrix[rank] += VectorPiRank{file} & VectorPiMask{pi};
    }

    void clear(Pi pi, Square sq) {
        matrix[Rank(sq)] -= VectorPiRank{File(sq)} & VectorPiMask{pi};
    }

    const VectorPiRank& operator[] (Rank rank) const {
        return matrix[rank];
    }

    VectorPiRank& operator[] (Rank rank) {
        return matrix[rank];
    }

    //pieces affecting the given square
    VectorPiMask operator[] (Square sq) const {
        return matrix[Rank{sq}][File{sq}];
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

    void filter(Pi pi, Bb b) {
        VectorPiMask exceptPi{ ::vectorOfAll[0xff] ^ ::vectorPiSingle[pi] };
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= VectorPiRank{b[rank]} | exceptPi;
        }
    }

    friend MatrixPiBb operator % (const MatrixPiBb& from, Bb b) {
        MatrixPiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] % VectorPiRank{b[rank]};
        }
        return result;
    }

    void operator &= (Bb b) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= VectorPiRank{b[rank]};
        }
    }

    friend MatrixPiBb operator & (const MatrixPiBb& from, Bb b) {
        MatrixPiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] & VectorPiRank{b[rank]};
        }
        return result;
    }

    index_t count() const {
        VectorBitCount::_t result = ::vectorOfAll[0];
        FOR_INDEX(Rank, rank) {
            result = _mm_add_epi8(result, bit_count.per_byte(static_cast<VectorBitCount::_t>(matrix[rank])));
        }
        return bit_count.total(result);
    }

    void setSliderAttack(Pi pi, SliderType ty, Square sq, ReverseBb blockers) {
        set(pi, blockers.attack(ty, sq));
    }

};

#endif
