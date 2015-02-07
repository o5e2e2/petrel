#include "SearchThread.hpp"
#include "Search.hpp"

SearchThread::sequence_t SearchThread::start(SearchFn* s, SearchControl* c, const Position* p, depth_t d) {
    searchFn = s;
    control = c;
    parent = p;
    draft = d;
    return commandRun();
}

void SearchThread::thread_body() {
    (*searchFn)(*control, *parent, draft);
}
