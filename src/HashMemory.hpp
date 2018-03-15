#ifndef HASH_MEMORY_HPP
#define HASH_MEMORY_HPP


#include "Zobrist.hpp"

class HashMemory {
public:

    struct Info {
        size_t currentSize = 0;
        size_t maxSize = 0;
    };

private:
    const size_t BucketSize;
    const size_t PageSize;

    void* hash = nullptr;
    std::uintptr_t mask;

    Info info = {0, 0};

    HashMemory (const HashMemory&) = delete;
    HashMemory& operator = (const HashMemory&) = delete;

    void set(void*, size_t);
    void setDefault();
    void free();

public:
    HashMemory (size_t bucketSize);
   ~HashMemory () { free(); }

    const Info& getInfo() const { return info; }

    size_t getTotalRecords() const { return info.currentSize / BucketSize; }

    void resize(size_t bytes);
    void clear();

    void* seek(Zobrist z) const;
    template <size_t bucketSize = 64> void* prefetch(Zobrist z) const;
};

#endif
