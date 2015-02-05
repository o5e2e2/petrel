#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchThread.hpp"
#include "SearchInfo.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"

class Node;
class SearchLimit;
class Position;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    SearchInfo info;

private:
    TranspositionTable transpositionTable;
    SearchThread searchThread;
    SearchThread::sequence_t sequence;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl ();
    void clear();

    //callbacks from search thread
    bool checkQuota() { return info.checkQuota(searchThread); }
    void report_bestmove() { info.report_bestmove(); }
    void report_perft_divide() { info.report_perft_divide(); }
    void report_perft_depth() { info.report_perft_depth(); }

    bool isReady() const { return searchThread.isReady(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(sequence); wait(); }

    TranspositionTable& tt() { return transpositionTable; }
    const TranspositionTable& tt() const { return transpositionTable; }

    void go(SearchOutput&, const Position&, const SearchLimit&);
};

#endif
