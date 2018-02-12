#include "SearchThread.hpp"
#include "Node.hpp"

void SearchThread::set(std::unique_ptr<Node> p) {
    assert( isReady() );
    assert( !parent );
    parent = p.release();
}

void SearchThread::thread_body() {
    if (!parent) {
        return;
    }

    parent->visitChildren();
    delete parent;
    parent = nullptr;
}
