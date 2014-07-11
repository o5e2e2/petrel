#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "io.hpp"

#include "Clock.hpp"
#include "Move.hpp"
#include "Node.hpp"
#include "Position.hpp"
#include "SearchThread.hpp"
#include "Timer.hpp"
#include "TranspositionTable.hpp"

class Uci;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
    TranspositionTable transpositionTable;

    SearchThread searchThread;
    Node* root;
    Uci* uci;

    enum { TickLimit = 1000 };
    node_count_t nodeLimit;

    node_count_t totalNodes;
    node_count_t counter[2];

    Timer moveTimer;
    mutable Clock clock;

    static const Ply MaxDepth = 1000;
    Ply depthLimit;

public:
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    SearchControl ();
   ~SearchControl () { stop(); }

    void clear();

    //callbacks from search thread
    void acquireNodesQuota(Node::quota_t&);
    void releaseNodesQuota(Node::quota_t&);

    void report_bestmove(Move);
    void report_perft(Move, index_t, node_count_t) const;
    void report_perft_depth(Ply depth, node_count_t);

    void count(index_t n) { ++counter[n]; }
    void clear_count() { counter[0] = counter[1] = 0; }

    bool isReady() const { return searchThread.isReady(); }
    bool isStopped() { return searchThread.isStopped(); }
    void wait() { searchThread.waitReady(); }
    void stop() { searchThread.commandStop(); wait(); }

    TranspositionTable::megabytes_t ttSize() { return transpositionTable.getSize(); }
    TranspositionTable::megabytes_t ttMaxSize() { return transpositionTable.getMaxSize(); }
    TranspositionTable::megabytes_t ttResize(TranspositionTable::megabytes_t s) { return transpositionTable.resize(s); }

    void go(Uci&, std::istream&, const Position&, Color);
};

extern SearchControl The_game;

#endif
