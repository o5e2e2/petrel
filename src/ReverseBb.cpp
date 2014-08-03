#include "ReverseBb.hpp"

ReverseBb::_t ReverseBb::combine(Bb lo, Bb hi) {
    return _mm_unpacklo_epi64( _mm_cvtsi64_si128(static_cast<__int64>(lo)), _mm_cvtsi64_si128(static_cast<__int64>(hi)) );
}

ReverseBb::Singleton::Singleton () {
    FOR_INDEX(Square, sq) {
        Square r_sq(~File{sq}, ~Rank{sq});

        (*this)[sq] = ReverseBb::combine(Bb{sq}, Bb{r_sq});
    }
}

ReverseBb::Direction::Direction () {
    FOR_INDEX(Square, sq) {
        Square r_sq{~File(sq), ~Rank(sq)};

        (*this)[Queen][sq][Horizont] = ReverseBb::combine(Bb::horizont(sq), Bb::horizont(r_sq));
        (*this)[Queen][sq][Vertical] = ReverseBb::combine(Bb::vertical(sq), Bb::vertical(r_sq));
        (*this)[Queen][sq][Diagonal] = ReverseBb::combine(Bb::diagonal(sq), Bb::diagonal(r_sq));
        (*this)[Queen][sq][Antidiag] = ReverseBb::combine(Bb::antidiag(sq), Bb::antidiag(r_sq));

        (*this)[Rook][sq][Horizont] = (*this)[Queen][sq][Horizont];
        (*this)[Rook][sq][Vertical] = (*this)[Queen][sq][Vertical];

        (*this)[Bishop][sq][Diagonal] = (*this)[Queen][sq][Diagonal];
        (*this)[Bishop][sq][Antidiag] = (*this)[Queen][sq][Antidiag];

        (*this)[Rook][sq][Diagonal] =
        (*this)[Rook][sq][Antidiag] =
        (*this)[Bishop][sq][Horizont] =
        (*this)[Bishop][sq][Vertical] = _mm_setzero_si128();
    }
}

ReverseBb::BitReverse::BitReverse () {
    FOR_INDEX (Index<0x10>, i) {
        reinterpret_cast<char*>(&lo_nibble_mask)[i] = 0x0f;
        reinterpret_cast<char*>(&byte_reverse)[i] = static_cast<char>(0x0f ^ i);

        //http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
        std::uint32_t n{ static_cast<std::uint32_t>(i) };
        std::uint32_t reverse{ ((n*0x0802 & 0x22110)|(n*0x8020 & 0x88440)) * 0x10101000 >> 24 };

        reinterpret_cast<char*>(&lo_nibble_reverse)[i] = static_cast<char>(reverse << 4);
        reinterpret_cast<char*>(&hi_nibble_reverse)[i] = static_cast<char>(reverse);
    }
}


