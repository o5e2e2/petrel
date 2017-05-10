#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"
#include "SearchOutput.hpp"

class SearchThread;

enum { PerftNodes, PerftDivideNodes, TT_Tried, TT_Hit, TT_Written, _Total };

class SearchInfo : public SearchOutput {
protected:
    typedef ::Index<_Total> Index;
    Index::array<node_count_t> _v;

    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    typedef signed quota_t;
    enum { TickLimit = 5000 }; // ~1 msec
    quota_t nodesQuota; //number of remaining nodes before checking for terminate

    Clock clock;
    depth_t depth; //current search depth

    Move _bestmove;
    Move currmove;
    index_t currmovenumber;

public:
    SearchInfo () { clear(); }

    node_count_t getNodes() const { return nodes - nodesQuota; }
    void setNodesLimit(node_count_t n) { nodesLimit = n; }

    void decrementQuota() { --nodesQuota; }
    bool checkQuota(const SearchThread&);

    void clear();
    void report_perft_divide(Move);
    void report_perft_depth(depth_t);
    void report_bestmove();

    void clearNodes() { _v = {0, 0, 0, 0, 0}; }
    const node_count_t&  operator [] (Index i) const { return _v[i]; }
    node_count_t&        operator [] (Index i)       { return _v[i]; }

};

#endif
