#ifndef SEARCH_THREAD_HPP
#define SEARCH_THREAD_HPP

#include "ThreadControl.hpp"

class Node;
class Position;

class SearchThread : public ThreadControl {
    Node* node;
    const Position* pos;

public:
    SearchThread ();
    void start(Node&, const Position&);
    void thread_body() override;
};

#endif
