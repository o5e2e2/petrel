#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>

typedef std::uint32_t U32;
typedef std::uint64_t U64;

#if !defined _WIN32
typedef std::int32_t __int32;
typedef std::int64_t __int64;
#endif

typedef int index_t; //small numbers [0..N) with a known upper bound
typedef index_t Ply; //search tree depth (relative to root)
typedef std::int64_t node_count_t;

#endif
