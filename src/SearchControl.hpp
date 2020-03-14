#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "NodeCounter.hpp"
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

    NodeCounter nodeCounter;
    SearchThread searchThread;

    PerftTT transpositionTable;
    Timer timer;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (UciSearchInfo&);

    void newGame();

    bool isBusy() const { return !searchThread.isIdle(); }
    bool isStopped() const { return searchThread.isStopped(); }
    void stop() { searchThread.stop(); }

    void uciok() const;
    void readyok() const;
    void infoPosition() const;
    void go(const SearchLimit&);
    void setHash(size_t);

    Duration getThinkingTime(const SearchLimit&) const;

    const PerftTT& tt() const { return transpositionTable; }
    PerftTT& tt() { return transpositionTable; }

    void nextIteration();

    void bestmove(const Move&, Score) const;
    void perftDepth(depth_t, node_count_t, const Move&, Score) const;
    void perftMove(index_t moveCount, const Move& currentMove, node_count_t, const Move&, Score) const;

    Control countNode();

};

#endif
