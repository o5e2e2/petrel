#ifndef VECTOR_PI_RANK_HPP
#define VECTOR_PI_RANK_HPP

#include "typedefs.hpp"
#include "Bb.hpp"
#include "Square.hpp"
#include "VectorBitCount.hpp"
#include "VectorOf.hpp"
#include "VectorPiBit.hpp"
#include "VectorPiSingle.hpp"

struct VectorPiRank : VectorPiBit<VectorPiRank, File> {
    constexpr VectorPiRank () : VectorPiBit{} {}
    explicit VectorPiRank (BitRank br) : VectorPiBit{::vectorOfAll[br]} {}
    explicit VectorPiRank (File f) : VectorPiBit{::vectorOfAll[BitRank{f}]} {}
    VectorPiRank (VectorPiMask m) : VectorPiBit{static_cast<VectorPiMask::_t>(m)} {}

    BitRank gather() const {
        _t v = this->_v;
        v |= _mm_unpackhi_epi64(v, v); //64
        v |= _mm_shuffle_epi32(v, _MM_SHUFFLE(1, 1, 1, 1)); //32
        v |= _mm_shufflelo_epi16(v, _MM_SHUFFLE(1, 1, 1, 1)); //16
        v |= _mm_srli_epi16(v, 8); //8
        return BitRank{small_cast<BitRank::_t>(_mm_cvtsi128_si32(v))};
    }

    constexpr BitRank operator [] (Pi pi) const {
        return BitRank{ VectorPiBit::operator[](pi) };
    }

    VectorPiMask operator [] (File file) const {
        _t file_vector = ::vectorOfAll[BitRank{file}];
        return _mm_cmpeq_epi8(file_vector, this->_v & file_vector);
    }

};

#endif
