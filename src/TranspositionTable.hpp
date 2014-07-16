#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include <new>
#include "bitops.hpp"
#include "Vector.hpp"
#include "Zobrist.hpp"

class TranspositionTable {
public:
    class CACHE_ALIGN Cluster {
        __m128i _t[4];

    public:
        Cluster () { _t[0] = _t[1] = _t[2] = _t[3] = _mm_setzero_si128(); }
    };

    typedef std::size_t size_type;

private:
    static const size_type MaxHash = static_cast<size_type>(4096) * 1024 * 1024; //4Gb

    char* hash;
    size_type size;
    size_type mask;

    static size_type round(size_type bytes) {
        return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)+1):0;
    }

    TranspositionTable (const TranspositionTable&) = delete;
    TranspositionTable& operator = (const TranspositionTable&) = delete;

public:
    TranspositionTable () : hash{nullptr}, size{0}, mask{0} {}
   ~TranspositionTable () { delete[] hash; }

    size_type getMaxSize() const { return MaxHash; }
    size_type getSize() const { return size; }

    size_type resize(size_type bytes) {
        bytes = std::min(bytes, getMaxSize());
        bytes = round(bytes);

        delete[] hash;
        size = 0;

        hash = nullptr;
        while (hash == nullptr && bytes > 0) {
            hash = reinterpret_cast<char*>(new (std::nothrow) Cluster[bytes/sizeof(Cluster)]);
            bytes >>= 1;
        }
        clear();

        size = bytes;
        mask = (bytes-1) ^ (sizeof(Cluster)-1);

        return size;
    }

    void clear() {
        if (size > 0) {
            std::memset(hash, 0, size);
        }
    }

    char* lookup(Zobrist z) const {
        return &(hash[static_cast<Zobrist::_t>(z) & mask]);
    }

};

#endif
