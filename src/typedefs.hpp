#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>

typedef int index_t; //small numbers [0..N) with a known upper bound
typedef index_t depth_t; //search tree depth (relative to root)
typedef std::int64_t node_count_t;
typedef signed node_quota_t;

#endif
