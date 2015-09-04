#ifndef SEARCH_WINDOW_HPP
#define SEARCH_WINDOW_HPP

#include "typedefs.hpp"
#include "Search.hpp"

struct SearchWindow {
    SearchControl& control;
    SearchFn* searchFn;
    depth_t draft;

    SearchWindow (SearchControl& c) : control(c), searchFn{nullptr}, draft{0} {}
    SearchWindow (const SearchWindow& w) : control(w.control), searchFn{w.searchFn}, draft{w.draft-1} {}
};

#endif
