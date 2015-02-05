#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"
#include "Node.hpp"

class SearchThread : public ThreadControl {
    SearchFn* searchFn;
    SearchControl* control;
    const Position* parent;
    depth_t draft;

    void thread_body() override;

public:
    sequence_t start(SearchFn, SearchControl*, const Position*, depth_t);
};

#endif
