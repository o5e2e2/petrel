#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"
#include "Search.hpp"

class SearchThread : public ThreadControl {
    SearchFn* searchFn;
    const Position* parent;
    SearchWindow* window;

    void thread_body() override;

public:
    void set(SearchFn*, const Position&, SearchWindow&);
};

#endif
