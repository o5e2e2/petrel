#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"

class Node;

class SearchThread : public ThreadControl {
    std::unique_ptr<Node> node;

    void thread_body() override;

public:
    SearchThread ();
    ~SearchThread();

    ThreadControl::Sequence start(std::unique_ptr<Node>);
};

#endif
