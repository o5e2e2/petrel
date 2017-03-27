#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "io.hpp"

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
public:
    SearchInfo info;

private:
    SearchThread     searchThread;
    SearchThread::_t searchSequence;
    SearchWindow     rootWindow;

    HashMemory transpositionTable;
    Timer::Pool timerPool;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (SearchOutput&);
    void clear();

    bool isReady() const { return searchThread.isReady(); }
    void stop() { searchThread.commandStop(searchSequence); searchThread.waitReady(); }

    const HashMemory& tt() const { return transpositionTable; }

    void uciSetHash(std::istream&);
    void go(const PositionMoves&, const SearchLimit&);

    //callbacks from search thread
    bool checkQuota() { return info.checkQuota(searchThread); }
    void nextIteration();

};

#endif
