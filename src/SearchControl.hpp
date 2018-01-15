#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchInfo.hpp"
#include "SearchLimit.hpp"
#include "SearchThread.hpp"

#include "HashMemory.hpp"
#include "Timer.hpp"

class Node;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    SearchInfo& info; //virtual
    SearchLimit searchLimit;

private:
    SearchThread     searchThread;
    SearchThread::_t searchSequence;

    Node* root;  //virtual

    HashMemory transpositionTable;
    Timer::TimerPool timerPool;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (SearchInfo&);
   ~SearchControl () { stop(); clear(); }

    void clear();
    void newGame();

    bool isReady() const { return searchThread.isReady(); }
    void stop() { searchThread.stop(searchSequence); }

    const HashMemory& tt() const { return transpositionTable; }
    HashMemory& tt() { return transpositionTable; }

    void go();

    //callbacks from search thread
    bool countNode();
    void nextIteration();

};

#endif
