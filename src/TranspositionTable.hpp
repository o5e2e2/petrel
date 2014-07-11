#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include <new>
#include "bitops.hpp"
#include "Vector.hpp"
#include "Zobrist.hpp"

class TranspositionTable {
    enum { MaxHash = 4096 }; //4Gb

public:
    class CACHE_ALIGN Cluster {
        __m128i _t[4];

    public:
        Cluster () { _t[0] = _t[1] = _t[2] = _t[3] = _mm_setzero_si128(); }
    };

    typedef std::size_t size_type;

private:
    Cluster* hash;
    size_type size;

    static size_type round(size_type bytes) {
        return (bytes > 0)? ::singleton<decltype(bytes)>(::bsr(bytes)+1):0;
    }

public:
    TranspositionTable () : hash{nullptr}, size{0} {}
   ~TranspositionTable () { delete[] hash; }

    size_type getMaxSize() const { return MaxHash; }
    size_type getSize() const { return size; }

    size_type resize(size_type bytes) {
        bytes = std::min(bytes, static_cast<decltype(bytes)>(MaxHash));
        bytes = round(bytes);

        delete[] hash;
        size = 0;
        hash = nullptr;

        while (hash == nullptr && bytes > 0) {
            hash = new (std::nothrow) Cluster[bytes/sizeof(Cluster)];
            bytes >>= 1;
        }
        clear();
        size = bytes;

        return size;
    }

    void clear() {
        std::memset(hash, 0, size);
    }

    void prefetch(Zobrist) {
    }

    Cluster* lookup(Zobrist) {
        return nullptr;
    }

    void write(Zobrist, Cluster&) {
        //hash[z] = c;
    }

};

#endif
