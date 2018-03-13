#ifndef BIT_OPS_HPP
#define BIT_OPS_HPP

#include <cstdint>

#if defined _MSC_VER
#   include <intrin.h>
#endif

#if defined _M_X64 || defined _M_AMD64 || defined __x86_64__ || defined __amd64__
#   define PLATFORM_64
#endif

#if !defined _WIN32
    typedef std::int32_t __int32;
    typedef std::int64_t __int64;
#endif

#if defined __GNUC__
#   define INLINE       inline __attribute__((__always_inline__))
#   define NOINLINE     __attribute__((__noinline__))
#   define CACHE_ALIGN  __attribute__((__aligned__(64)))
#elif defined _MSC_VER
#   define INLINE       inline __forceinline
#   define NOINLINE     __declspec(noinline)
#   define CACHE_ALIGN  __declspec(align(64))
#else
#   define INLINE inline
#   define NOINLINE
#   define CACHE_ALIGN
#endif

#if !defined NDEBUG
#   define ASSERT_INDEX_CHECK
//#   define ASSERT_DEBUG_BREAK
#endif

#if defined NDEBUG
#   define assert(test) ((void)0)
#elif defined ASSERT_DEBUG_BREAK && defined _MSC_VER
#   pragma intrinsic(__debugbreak)
#   if defined assert
#       undef assert
#   endif
#   define assert(test) (void)( (!!(test)) || (__debugbreak(), 0) )
#elif defined ASSERT_DEBUG_BREAK && defined __GNUC__
#   define assert(test) do { if (!(test)) __asm__("int $3"); } while(0)
#else
#   include <cassert>
#endif

template <typename T>
constexpr T universe() { return static_cast<T>(~static_cast<T>(0)); }

template <typename T, typename N>
constexpr T small_cast(N n) { return static_cast<T>(n & static_cast<N>(universe<T>())); }

template <typename T, typename N>
constexpr T singleton(N n) { return static_cast<T>(static_cast<T>(1) << n); }

//TRICK: "v & (v-1)" clears the lowest unset bit
template <typename T>
constexpr T withoutLsb(T n) { return n & static_cast<T>(n-1); }

template <typename T>
constexpr bool isSingleton(T n) { return (n != 0) && (::withoutLsb(n) == 0); }

typedef int index_t; //small numbers [0..N) with a known upper bound

#if defined __GNUC__
    INLINE index_t bsf(std::uint32_t b) {
        assert (b != 0);
        return __builtin_ctz(b);
    }

    INLINE index_t bsr(std::uint32_t b) {
        assert (b != 0);
        index_t i;
        __asm__ ( "bsrl %1, %0" : "=Ir" (i) : "rm" (b) : "cc" );
        return i;
    }

    INLINE std::uint32_t bswap(std::uint32_t b) {
        return __builtin_bswap32(b);
    }

#   if defined PLATFORM_64
        INLINE index_t bsf(std::uint64_t b) {
            assert (b != 0);
            return __builtin_ctzll(b);
        }

        INLINE index_t bsr(std::uint64_t b) {
            assert (b != 0);
            __asm__ ( "bsrq %1, %0" : "=Jr" (b) : "rm" (b) : "cc" );
            return static_cast<index_t>(b);
        }

        INLINE std::uint64_t bswap(std::uint64_t b) {
            return __builtin_bswap64(b);
        }

#   endif

#elif defined _MSC_VER
#   pragma intrinsic(_BitScanForward)
    INLINE index_t bsf(std::uint32_t b) {
        unsigned long result;
        assert (b != 0);
        _BitScanForward(&result, b);
        return result;
    }

#   pragma intrinsic(_BitScanReverse)
    INLINE index_t bsr(std::uint32_t b) {
        unsigned long result;
        assert (b != 0);
        _BitScanReverse(&result, b);
        return result;
    }

#   pragma intrinsic(_byteswap_ulong)
    INLINE std::uint32_t bswap(std::uint32_t b) {
        return _byteswap_ulong(b);
    }

#   if defined PLATFORM_64
#       pragma intrinsic(_BitScanForward64)
        INLINE index_t bsf(std::uint64_t b) {
            unsigned long result;
            assert (b != 0);
            _BitScanForward64(&result, b);
            return result;
        }

#       pragma intrinsic(_BitScanReverse64)
        INLINE index_t bsr(std::uint64_t b) {
            unsigned long result;
            assert (b != 0);
            _BitScanReverse64(&result, b);
            return result;
        }

#       pragma intrinsic(_byteswap_uint64)
        INLINE std::uint64_t bswap(std::uint64_t b) {
            return _byteswap_uint64(b);
        }
#   endif
#else
#   error No bitscan intrinsics defined
    INLINE std::uint32_t bswap(std::uint32_t b) {
        return (b >> 24) | (b << 24)
               | ((b & 0x00ff0000) >>  8)
               | ((b & 0x0000ff00) <<  8)
               ;
    }
#endif

#if !defined PLATFORM_64

    constexpr unsigned __int32 lo(unsigned __int64 b) {
        return small_cast<unsigned __int32>(b);
    }

    constexpr unsigned __int32 hi(unsigned __int64 b) {
        return static_cast<unsigned __int32>(b >> 32);
    }

    constexpr unsigned __int64 combine(unsigned __int32 lo, unsigned __int32 hi) {
        return (static_cast<unsigned __int64>(hi) << 32) | static_cast<unsigned __int64>(lo);
    }

    constexpr __int32 lo(__int64 b) {
        return static_cast<__int32>( ::lo(static_cast<unsigned __int64>(b)) );
    }

    constexpr __int32 hi(__int64 b) {
        return static_cast<__int32>( ::hi(static_cast<unsigned __int64>(b)) );
    }
    constexpr __int64 combine(__int32 lo, __int32 hi) {
        return static_cast<__int64>( ::combine(static_cast<unsigned __int32>(lo), static_cast<unsigned __int32>(hi)) );
    }

    INLINE index_t bsf(std::uint64_t b) {
        assert (b != 0);
        auto lo = ::lo(b);
        return ::bsf(lo ? lo : (hi(b)+32));
    }

    INLINE index_t bsr(std::uint64_t b) {
        assert (b != 0);
        auto hi = ::hi(b);
        return ::bsr(hi ? (hi+32) : lo(b));
    }

    INLINE std::uint64_t bswap(std::uint64_t b) {
        return ::combine( bswap(::hi(b)), bswap(::lo(b)) );
    }
#endif

#endif
