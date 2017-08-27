#include "SearchThread.hpp"
#include "Node.hpp"

void SearchThread::set(Node* p, Node* c) {
    assert( isReady() );

    if (isReady()) {
        parent = p;
        child = c;
    }
}

void SearchThread::thread_body() {
    if (parent && child) {
        parent->visitChildren(*child);
    }
}
