#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include <new>
#include "Vector.hpp"
#include "Zobrist.hpp"

class TranspositionTable {
    class CACHE_ALIGN Cluster {
        __m128i _t[4];
    };

    typedef std::size_t size_type;

    static size_type round(size_type bytes);
    void setMaxSize();

    Cluster* hash;
    size_type mask;
    size_type size;
    size_type maxSize;

    Cluster one;

    TranspositionTable (const TranspositionTable&) = delete;
    TranspositionTable& operator = (const TranspositionTable&) = delete;

    void free() {
        if (size > sizeof(Cluster)) {
            delete[] hash;
        }
        hash = &one;
        size = sizeof(Cluster);
        mask = 0;
    }

public:
    TranspositionTable () : hash{&one}, mask{0}, size{sizeof(Cluster)} { setMaxSize(); }
   ~TranspositionTable () { free(); }

    size_type getMaxSize() const { return maxSize; }
    size_type getSize() const { return size; }

    size_type setSize(size_type bytes) {
        if (size == bytes) {
            return size; //do nothing
        }

        free();
        setMaxSize();

        if (bytes <= sizeof(Cluster)) {
            assert (size == sizeof(Cluster));
            assert (mask == 0);
            return size;
        }

        bytes = std::min(bytes, maxSize);
        bytes = round(bytes);

        for (; bytes > sizeof(Cluster); bytes >>= 1) {
            hash = reinterpret_cast<decltype(hash)>(new (std::nothrow) Cluster[bytes/sizeof(Cluster)]);

            if (hash != nullptr) {
                size = bytes;
                break;
            }
        }

        if (hash == nullptr) {
            hash = &one;
        }

        clear();
        mask = (size-1) ^ (sizeof(Cluster)-1);
        return size;
    }

    void clear() {
        std::memset(hash, 0, size);
    }

    char* lookup(Zobrist z) {
        return &(reinterpret_cast<char*>(hash)[static_cast<Zobrist::_t>(z) & mask]);
    }

};

#endif
