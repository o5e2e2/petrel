#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchInfo.hpp"
#include "SearchThread.hpp"
#include "SearchWindow.hpp"
#include "Timer.hpp"
#include "HashMemory.hpp"

class SearchLimit;
class Position;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    SearchThread searchThread;
    SearchThread::sequence_t sequence;
    SearchWindow rootWindow;
    mutable SearchInfo info;

    HashMemory transpositionTable;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl ();
    void clear();

    bool isReady() const { return searchThread.isReady(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(sequence); wait(); }

    HashMemory& tt() { return transpositionTable; }
    const HashMemory& tt() const { return transpositionTable; }

    void go(SearchOutput&, const Position&, const SearchLimit&);

    //callbacks from search thread
    bool checkQuota() const { return info.checkQuota(searchThread); }
    void decrementQuota() const { info.decrementQuota(); }
    void report_bestmove() const { info.report_bestmove(); }
    void report_perft_depth(depth_t draft) const { info.report_perft_depth(draft); }
    void report_perft_divide(Move move) const { info.report_perft_divide(move); }

    node_count_t getPerftNodes() const { return info.perftNodes; }
    void addPerftNodes(node_count_t n) const { info.perftNodes += n; }

};

#endif
