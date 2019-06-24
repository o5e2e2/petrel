#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"

class Node;

class SearchThread : public ThreadControl {
    std::unique_ptr<Node> node;

    void run() override;

public:
    SearchThread ();
    ~SearchThread();

    TaskId start(std::unique_ptr<Node>);
};

#endif
