#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchThread.hpp"
#include "PerftTT.hpp"
#include "Timer.hpp"

class Move;
class SearchLimit;
class UciSearchInfo;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    UciSearchInfo& info; //virtual

    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    typedef unsigned nodes_quota_t;
    enum : nodes_quota_t { TickLimit = 5000 }; // ~1 msec
    nodes_quota_t nodesQuota; //number of remaining nodes before slow checking for terminate

    SearchThread searchThread;

    PerftTT transpositionTable;
    Timer timer;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

    bool refreshQuota();

public:
    SearchControl (UciSearchInfo&);

    /**
     * Running search uses pointer this object, so we should stop any running search
     **/
   ~SearchControl () { stop(); }

    void newGame();

    bool isReady() const { return searchThread.isReady(); }
    bool isStopped() const { return searchThread.isStopped(); }
    void stop() { searchThread.stop(); }

    void uciok() const;
    void readyok() const;
    void go(const SearchLimit&);

    const PerftTT& tt() const { return transpositionTable; }
    PerftTT& tt() { return transpositionTable; }

    void nextIteration();

    void bestmove(const Move&, Score) const;
    void perftDepth(depth_t, node_count_t, const Move&, Score) const;
    void perftMove(index_t moveCount, const Move& currentMove, node_count_t, const Move&, Score) const;

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
