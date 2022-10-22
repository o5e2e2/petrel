#ifndef PI_BB_H
#define PI_BB_H

#include "Bb.hpp"
#include "PiRank.hpp"

/// array of 8 PiRank
class PiBb {
    Rank::arrayOf<PiRank> matrix;

public:
    constexpr PiBb () = default;

    friend void swap(PiBb& a, PiBb& b) {
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
        matrix[Rank(sq)] -= PiRank{File(sq)} & PiMask{pi};
    }

    void clear(Pi pi) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] %= PiMask{pi};
        }
    }

    void set(Pi pi, Rank rank, BitRank br) {
        //_mm_blendv_epi8
        matrix[rank] = (matrix[rank] % PiMask{pi}) + (PiRank{br} & PiMask{pi});
    }

    void set(Pi pi, Bb bb) {
        FOR_INDEX(Rank, rank) {
            set(pi, rank, bb[rank]);
        }
    }

    void add(Pi pi, Rank rank, File file) {
        matrix[rank] += PiRank{file} & PiMask{pi};
    }

    void add(Pi pi, Square sq) {
        add(pi, Rank(sq), File(sq));
    }

    bool has(Pi pi, Square sq) const {
        return (matrix[Rank(sq)] & PiRank{File(sq)} & PiMask{pi}).any();
    }

    const PiRank& operator[] (Rank rank) const {
        return matrix[rank];
    }

    PiRank& operator[] (Rank rank) {
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
        PiMask exceptPi{ PiMask::all() ^ PiMask{pi} };
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= PiRank{bb[rank]} | exceptPi;
        }
    }

    friend PiBb operator % (const PiBb& from, Bb bb) {
        PiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] % PiRank{bb[rank]};
        }
        return result;
    }

    void operator &= (Bb bb) {
        FOR_INDEX(Rank, rank) {
            matrix[rank] &= PiRank{bb[rank]};
        }
    }

    friend PiBb operator & (const PiBb& from, Bb bb) {
        PiBb result;
        FOR_INDEX(Rank, rank) {
            result.matrix[rank] = from.matrix[rank] & PiRank{bb[rank]};
        }
        return result;
    }

    index_t count() const {
        VectorBitCount::_t sum = ::vectorOfAll[0];
        FOR_INDEX(Rank, rank) {
            sum = _mm_add_epi8(sum, ::bitCount.bytes( static_cast<VectorBitCount::_t>(matrix[rank]) ));
        }
        return ::bitCount.total(sum);
    }

};

#endif
