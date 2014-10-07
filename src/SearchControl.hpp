#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "Clock.hpp"
#include "Move.hpp"
#include "SearchThread.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"

class Node;
class SearchOutput;
class SearchLimit;
class Position;

struct SearchInfo {
    node_count_t nodes;
    duration_t duration;
};

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    TranspositionTable transpositionTable;
    SearchThread searchThread;
    Node* root;
    SearchOutput* out;

    enum { TickLimit = 1000 };
    node_count_t nodeLimit;

    static const depth_t MaxDepth = 1000;
    depth_t depthLimit;

    Timer moveTimer;

    node_count_t totalNodes;
    mutable Clock clock;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl ();

    void clear();

    //callbacks from search thread
    void acquireNodesQuota(node_quota_t&);
    void releaseNodesQuota(node_quota_t&);

    void report_bestmove(Move);
    void report_perft(Move, index_t, node_count_t) const;
    void report_perft_depth(depth_t depth, node_count_t);

    bool isReady() const { return searchThread.isReady(); }
    bool isStopped() { return searchThread.isStopped(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(); wait(); }

    void getSearchInfo(SearchInfo&) const;

    TranspositionTable& tt() { return transpositionTable; }

    void go(SearchOutput&, const Position&, const SearchLimit&);
};

extern SearchControl The_game;

#endif
