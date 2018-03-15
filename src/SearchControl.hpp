#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchThread.hpp"
#include "PerftTT.hpp"
#include "Timer.hpp"

class UciSearchInfo;
class SearchLimit;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    UciSearchInfo& info; //virtual

private:
    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    enum : unsigned { TickLimit = 5000 }; // ~1 msec
    unsigned nodesQuota; //number of remaining nodes before slow checking for terminate

    SearchThread searchThread;
    SearchThread::Sequence searchSequence;

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
    void stop() { searchThread.stop(searchSequence); }

    const PerftTT& tt() const { return transpositionTable; }
    PerftTT& tt() { return transpositionTable; }

    void go(const SearchLimit&);

    void nextIteration();

    node_count_t getNodes() const {
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
