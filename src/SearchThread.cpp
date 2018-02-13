#include "SearchThread.hpp"
#include "Node.hpp"

SearchThread::SearchThread() = default;
SearchThread::~SearchThread() = default;

void SearchThread::set(std::unique_ptr<Node> n) {
    assert (isReady());
    assert (!node);
    node = std::move(n);
}

void SearchThread::thread_body() {
    assert (node);
    node->visitChildren();
    node.release();
}
