#ifndef SPIN_LOCK_HPP
#define SPIN_LOCK_HPP

#include <atomic>
#include <immintrin.h>
#include <mutex>

class SpinLock {
    std::atomic_flag atomic = ATOMIC_FLAG_INIT;

public:
    typedef std::lock_guard<SpinLock> Guard;

    bool try_lock() {
        return !atomic.test_and_set(std::memory_order_acquire);
    }

    void lock() {
        while (atomic.test_and_set(std::memory_order_acquire)) {
            _mm_pause();
        }
    }

    void unlock() {
        atomic.clear(std::memory_order_release);
    }

};

#endif
