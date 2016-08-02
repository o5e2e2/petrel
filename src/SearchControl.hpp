#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchInfo.hpp"
#include "SearchThread.hpp"
#include "SearchWindow.hpp"

#include "HashMemory.hpp"
#include "Timer.hpp"

class PositionMoves;
class SearchLimit;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    SearchThread     searchThread;
    SearchThread::_t searchSequence;
    SearchWindow     rootWindow;

    HashMemory transpositionTable;
    Timer::Pool timerPool;
    
    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchInfo info;

    SearchControl ();
    void clear();

    bool isReady() const { return searchThread.isReady(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(searchSequence); wait(); }

    const HashMemory& tt() const { return transpositionTable; }
    void resizeHash(HashMemory::size_t bytes);

    void go(SearchOutput&, const PositionMoves&, const SearchLimit&);

    //callbacks from search thread
    bool checkQuota() { return info.checkQuota(searchThread); }

    void nextIteration();

};

#endif
