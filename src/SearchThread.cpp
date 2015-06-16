#include "SearchThread.hpp"
#include "Search.hpp"
#include "SearchWindow.hpp"

void SearchThread::set(SearchFn* s, const Position& p, SearchWindow& w) {
    assert(isReady());

    searchFn = s;
    parent = &p;
    window = &w;
}

void SearchThread::thread_body() {
    if (searchFn != nullptr) {
        (*searchFn)(*parent, *window);
    }
}
