#include "SearchThread.hpp"
#include "Node.hpp"

void SearchThread::start(SearchFn s, SearchControl* c, const Position* p, depth_t d) {
    searchFn = s;
    control = c;
    parent = p;
    draft = d;
    commandRun();
}

void SearchThread::thread_body() {
    searchFn(*control, *parent, draft);
}
