#include "SearchThread.hpp"
#include "Node.hpp"

void SearchThread::set(Node* p) {
    assert( isReady() );

    if (isReady()) {
        parent = p;
    }
}

void SearchThread::thread_body() {
    if (parent) {
        parent->visitChildren();
    }
}
