#include "SearchThread.hpp"
#include "Node.hpp"

SearchThread::SearchThread() = default;
SearchThread::~SearchThread() = default;

ThreadControl::ThreadId SearchThread::start(std::unique_ptr<Node> n) {
    assert (isReady());
    assert (!node);
    node = std::move(n);
    return ThreadControl::start();
}

void SearchThread::thread_body() {
    assert (node);
    node->visitChildren();
    node.release();
}
