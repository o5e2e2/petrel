#ifndef BIT_ARRAY_128_REVERSE_HPP
#define BIT_ARRAY_128_REVERSE_HPP

#include "bitops128.hpp"
#include "Index.hpp"

class BitReverse {
    typedef __m128i _t;
    _t lowNibbleReverse, highNibbleReverse, lowNibbleMask, byteReverse;

public:
    BitReverse () {
        FOR_INDEX (Index<0x10>, i) {
            reinterpret_cast<char*>(&lowNibbleMask)[i] = 0x0f;
            reinterpret_cast<char*>(&byteReverse)[i] = static_cast<char>(0x0f ^ i);

            //http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
            std::uint32_t n{ static_cast<std::uint32_t>(i) };
            std::uint32_t reverse{ ((n*0x0802 & 0x22110)|(n*0x8020 & 0x88440)) * 0x10101000 >> 24 };

            reinterpret_cast<char*>(&lowNibbleReverse)[i] = static_cast<char>(reverse << 4);
            reinterpret_cast<char*>(&highNibbleReverse)[i] = static_cast<char>(reverse);
        }
    }

    _t bitSwap(_t v) const {
        _t hi = _mm_shuffle_epi8(highNibbleReverse, lowNibbleMask & _mm_srli_epi16(v, 4));
        _t lo = _mm_shuffle_epi8(lowNibbleReverse, lowNibbleMask & v);
        return hi ^ lo;
    }

    _t byteSwap(_t v) const {
        return _mm_shuffle_epi8(v, byteReverse);
    }

    _t operator() (_t v) const {
        return byteSwap(bitSwap(v));
    }

};

extern const BitReverse bitReverse;

#endif
