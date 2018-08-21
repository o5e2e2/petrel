#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchLimit.hpp"
#include "SearchThread.hpp"
#include "PerftTT.hpp"
#include "Timer.hpp"

class UciSearchInfo;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    UciSearchInfo& info; //virtual

protected:
    SearchLimit searchLimit;

private:
    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    enum : unsigned { TickLimit = 5000 }; // ~1 msec
    unsigned nodesQuota; //number of remaining nodes before slow checking for terminate

    SearchThread searchThread;
    SearchThread::ThreadId currentSearchId;

    PerftTT transpositionTable;
    Timer timer;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

    bool refreshQuota();

public:
    SearchControl (UciSearchInfo&);
   ~SearchControl () { stop(); }

    void newGame();

    bool isReady() const { return searchThread.isReady(); }
    bool isStopped() const { return searchThread.isStopped(); }
    void stop() { searchThread.stop(currentSearchId); }

    const PerftTT& tt() const { return transpositionTable; }
    PerftTT& tt() { return transpositionTable; }

    void go();

    void nextIteration();

    node_count_t getNodesVisited() const {
        assert (nodes >= nodesQuota);
        return nodes - nodesQuota;
    }

    bool countNode() {
        if (nodesQuota > 0) {
            --nodesQuota;
            return false;
        }
        return refreshQuota();
    }

};

#endif
