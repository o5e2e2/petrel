#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchInfo.hpp"
#include "SearchThread.hpp"
#include "SearchWindow.hpp"
#include "HashMemory.hpp"

class SearchLimit;
class Position;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    SearchThread searchThread;
    SearchThread::_t searchSequence;
    SearchWindow rootWindow;
    HashMemory transpositionTable;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    mutable SearchInfo info;

    SearchControl ();
    void clear();

    bool isReady() const { return searchThread.isReady(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(searchSequence); wait(); }

    const HashMemory& tt() const { return transpositionTable; }
    void resizeHash(HashMemory::size_t bytes);

    void go(SearchOutput&, const Position&, const SearchLimit&);

    //callbacks from search thread
    bool checkQuota() const { return info.checkQuota(searchThread); }

};

#endif
