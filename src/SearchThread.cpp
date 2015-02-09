#include "SearchThread.hpp"
#include "Search.hpp"
#include "SearchWindow.hpp"

SearchThread::sequence_t SearchThread::start(SearchFn* s, const Position& p, SearchWindow& w) {
    searchFn = s;
    parent = &p;
    window = &w;
    return commandRun();
}

void SearchThread::thread_body() {
    if (searchFn != nullptr) {
        (*searchFn)(*parent, *window);
    }
}
