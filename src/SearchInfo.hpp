#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

class SearchOutput;
class SearchThread;

enum { TT_Tried, TT_Hit, TT_Used, _Total };

class SearchInfo {
    enum { TickLimit = 5000 }; // ~1 msec
    typedef signed node_quota_t;

private:
    typedef node_count_t _t;
    typedef ::Index<_Total> Index;
    Index::array<_t> _v;

public:
    node_quota_t nodesQuota; //number of remaining nodes before checking for terminate
    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    SearchOutput* out;
    Clock clock;
    depth_t depth; //current search depth

    Move bestmove;
    Move currmove;
    index_t currmovenumber;

    node_count_t perftNodes;
    node_count_t perftDivide;

private:
    void resetNodesQuota();
    void acquireNodesQuota();

public:
    void decrementQuota() { --nodesQuota; }
    bool checkQuota(const SearchThread&);

    void clear();
    void report_perft_divide(Move);
    void report_perft_depth(depth_t);
    void report_bestmove();

    void clearTT() { _v = {0, 0, 0}; }
    const _t&  operator [] (Index i) const { return _v[i]; }
    _t&        operator [] (Index i)       { return _v[i]; }

};

#endif
