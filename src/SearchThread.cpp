#include "SearchThread.hpp"
#include "Node.hpp"

SearchThread::SearchThread() = default;
SearchThread::~SearchThread() { stop(); }

ThreadControl::TaskId SearchThread::start(std::unique_ptr<Node> n) {
    assert (isIdle());
    assert (!node);

    node = std::move(n);
    return ThreadControl::start();
}

void SearchThread::run() {
    assert (isRunning());
    assert (node);

    node->visitChildren();
    node.release();
}
