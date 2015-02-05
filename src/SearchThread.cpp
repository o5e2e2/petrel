#include "SearchThread.hpp"
#include "Node.hpp"

SearchThread::sequence_t SearchThread::start(SearchFn s, SearchControl* c, const Position* p, depth_t d) {
    searchFn = s;
    control = c;
    parent = p;
    draft = d;
    return commandRunSequence();
}

void SearchThread::thread_body() {
    searchFn(*control, *parent, draft);
}
