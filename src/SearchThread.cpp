#include "SearchThread.hpp"
#include "Search.hpp"
#include "SearchWindow.hpp"

SearchThread::sequence_t SearchThread::start(SearchFn* s, const Position& p, const SearchControl& c, SearchWindow& w) {
    searchFn = s;
    parent = &p;
    control = &c;
    window = &w;
    return commandRun();
}

void SearchThread::thread_body() {
    if (searchFn != nullptr) {
        (*searchFn)(*parent, *control, *window);
    }
}
