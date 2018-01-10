#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchThread.hpp"

#include "HashMemory.hpp"
#include "Timer.hpp"

class SearchInfo;
class PositionMoves;
class SearchLimit;
class NodeRoot;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    SearchInfo& info; //virtual

private:
    SearchThread     searchThread;
    SearchThread::_t searchSequence;

    NodeRoot* root;

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
    void resizeHash(size_t bytes) { tt().resize(bytes); }

    void go(const SearchLimit&);

    //callbacks from search thread
    bool countNode();
    void nextIteration();

};

#endif
