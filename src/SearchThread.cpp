#include "SearchThread.hpp"
#include "Node.hpp"
#include "SearchControl.hpp"

SearchThread::SearchThread () : ThreadControl{}, node{} {}

void SearchThread::start(Node& n, const Position& p) {
    node = &n;
    pos = &p;
    commandRun();
}

void SearchThread::thread_body() {
    (*node)(*pos);
}
