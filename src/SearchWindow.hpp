#ifndef SEARCH_WINDOW_HPP
#define SEARCH_WINDOW_HPP

#include "typedefs.hpp"
#include "Search.hpp"

struct SearchWindow {
    SearchFn* searchFn;
    depth_t draft;

    SearchWindow () : searchFn{nullptr}, draft{0} {}
    SearchWindow (const SearchWindow& w) : searchFn{w.searchFn}, draft{w.draft-1} {}
};

#endif
