#include "SearchThread.hpp"
#include "Node.hpp"

SearchThread::SearchThread() = default;
SearchThread::~SearchThread() = default;

ThreadControl::RunId SearchThread::start(std::unique_ptr<Node> n) {
    assert (isReady());
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
