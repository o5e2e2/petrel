#ifndef SEARCH_CONTROL_HPP
#define SEARCH_CONTROL_HPP

#include "SearchThread.hpp"
#include "HashMemory.hpp"
#include "Timer.hpp"

class SearchInfo;
class SearchLimit;

/**
 * Shared data to all search threads (currently the only one)
 */
class SearchControl {
public:
    SearchInfo& info; //virtual

private:
    SearchThread searchThread;
    SearchThread::Sequence searchSequence;

    HashMemory transpositionTable;
    Timer timer;

    SearchControl (const SearchControl&) = delete;
    SearchControl& operator = (const SearchControl&) = delete;

public:
    SearchControl (SearchInfo&);
   ~SearchControl () { stop(); clear(); }

    void clear();
    void newGame();

    bool isReady() const { return searchThread.isReady(); }
    void stop() { searchThread.stop(searchSequence); }

    const HashMemory& tt() const { return transpositionTable; }
    HashMemory& tt() { return transpositionTable; }

    void go(const SearchLimit&);

    //callbacks from search thread
    bool countNode();
    void nextIteration();

};

#endif
