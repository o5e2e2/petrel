#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchInfo.hpp"
#include "SearchThread.hpp"
#include "SearchWindow.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"

class SearchLimit;
class Position;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    mutable SearchInfo info;

private:
    SearchThread searchThread;
    SearchThread::sequence_t sequence;
    SearchWindow window;

    TranspositionTable transpositionTable;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl ();
    void clear();

    //callbacks from search thread
    bool checkQuota() const { return info.checkQuota(searchThread); }

    bool isReady() const { return searchThread.isReady(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(sequence); wait(); }

    TranspositionTable& tt() { return transpositionTable; }
    const TranspositionTable& tt() const { return transpositionTable; }

    void go(SearchOutput&, const Position&, const SearchLimit&);
};

#endif
