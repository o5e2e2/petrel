#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "NodeCounter.hpp"
#include "SearchThread.hpp"
#include "Score.hpp"
#include "PerftTT.hpp"
#include "PvMoves.hpp"
#include "Timer.hpp"

class Move;
class SearchLimit;
class UciSearchInfo;

class SearchControl {
    UciSearchInfo& info; //virtual

    NodeCounter nodeCounter;
    SearchThread searchThread;

    PerftTT transpositionTable;
    Timer timer;

    PvMoves<DepthMax> pvMoves;

public:
    ply_t ply = 0; //distance to root of the current node

private:
    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (UciSearchInfo&);

    void newGame();

    bool isAborted() const { return nodeCounter.isAborted(); }

    bool isBusy() const { return !searchThread.isIdle(); }
    bool isStopped() const { return searchThread.isStopped(); }
    void stop() { searchThread.stop(); }

    void uciok() const;
    void isready() const;
    void infoPosition() const;
    void go(const SearchLimit&);
    void setHash(size_t);

    const PerftTT& tt() const { return transpositionTable; }
    PerftTT& tt() { return transpositionTable; }

    void createPv(const Move&);

    void nextIteration();

    void readyok() const;
    void bestmove(Score) const;
    void infoDepth(ply_t, Score) const;
    void infoPerftDepth(ply_t, node_count_t, Score) const;
    void infoPerftMove(index_t moveCount, const Move& currentMove, node_count_t, Score) const;

    NodeControl countNode();

};

#endif
