#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Move.hpp"
#include "SearchThread.hpp"
#include "SearchInfo.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"

class Node;
class SearchOutput;
class SearchLimit;
class Position;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    TranspositionTable transpositionTable;
    SearchThread searchThread;

public:
    SearchInfo info;

private:
    Node* root;
    SearchOutput* out;

    enum { TickLimit = 1000 };
    node_count_t nodeLimit;

    depth_t depthLimit;

    Timer moveTimer;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

    void acquireNodesQuota();

public:
    SearchControl ();

    void clear();

    //callbacks from search thread
    bool checkQuota() {
        if (info.nodesRemaining <= 0) {
            acquireNodesQuota();
        }
        return info.nodesRemaining <= 0;
    }

    void report_bestmove();
    void report_perft_divide();
    void report_perft_depth(depth_t depth);

    bool isReady() const { return searchThread.isReady(); }
    bool isStopped() { return searchThread.isStopped(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(); wait(); }

    TranspositionTable& tt() { return transpositionTable; }
    const TranspositionTable& tt() const { return transpositionTable; }

    void go(SearchOutput&, const Position&, const SearchLimit&);
};

#endif
