#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"

class Node;

class SearchThread : public ThreadControl {
    Node* parent;

    void thread_body() override;

public:
    SearchThread () : parent(nullptr) {}
    void set(std::unique_ptr<Node> parent);
};

#endif
