#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include <new>
#include "bitops.hpp"
#include "Vector.hpp"
#include "Zobrist.hpp"

class TranspositionTable {
    enum { MaxHash = 4096 }; //4Gb

public:
    typedef unsigned megabytes_t;

    class CACHE_ALIGN Cluster {
        __m128i _t[4];

    public:
        Cluster () { _t[0] = _t[1] = _t[2] = _t[3] = _mm_setzero_si128(); }
    };

    enum { PerMb = 1024*1024/sizeof(Cluster) }; //=16384

    typedef size_t size_type;

private:
    Cluster* hash;
    size_type size; //number of entries

    static unsigned round(megabytes_t mb) {
        return mb? ::singleton<decltype(mb)>(::bsr(mb)+1):0;
    }

public:
    TranspositionTable () : hash{nullptr}, size{0} {}
   ~TranspositionTable () { delete[] hash; }

    megabytes_t getMaxSize() const { return MaxHash; }
    megabytes_t getSize() const { return static_cast<megabytes_t>(size / PerMb); }

    megabytes_t resize(megabytes_t mb) {
        mb = std::min(mb, static_cast<decltype(mb)>(MaxHash));

        mb = round(mb);
        size_type elements = mb * PerMb;

        delete[] hash;
        hash = nullptr;

        while (hash == nullptr && elements > 0) {
            hash = new (std::nothrow) Cluster[elements];
            elements /= 2;
        }
        size = elements;

        clear();
        return static_cast<decltype(mb)>(elements / PerMb);
    }

    void clear() {
        std::memset(hash, 0, size * sizeof(Cluster));
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
