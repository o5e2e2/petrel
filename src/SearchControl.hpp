#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchThread.hpp"

#include "HashMemory.hpp"
#include "Timer.hpp"

class SearchInfo;
class PositionMoves;
class SearchLimit;
class NodeRoot;
class NodePerft;

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
    NodePerft* child;

    HashMemory transpositionTable;
    Timer::TimerPool timerPool;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (SearchInfo&);
    void clear();

    bool isReady() const { return searchThread.isReady(); }
    void stop() { searchThread.stop(searchSequence); }

    const HashMemory& tt() const { return transpositionTable; }
    HashMemory& tt() { return transpositionTable; }

    void go(const PositionMoves&, const SearchLimit&);

    //callbacks from search thread
    bool checkQuota();
    void nextIteration();

};

#endif
