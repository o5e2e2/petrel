#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "Clock.hpp"
#include "Move.hpp"

class SearchThread;

enum { PerftNodes, PerftDivideNodes, TT_Tried, TT_Hit, TT_Written, _Total };

class SearchInfo {
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
    void clearNodes() { _v = {0, 0, 0, 0, 0}; }

    void report_perft_divide(Move);
    void report_perft_depth(depth_t);

    node_count_t get(Index i) const { return _v[i]; }
    void inc(Index i, node_count_t n = 1) { _v[i] += n; }

    //do nothing by default
    virtual void bestmove() const {};
    virtual void readyok() const {};
    virtual void info_depth() const {};
    virtual void info_currmove() const {};
    virtual ~SearchInfo() {}
};

#endif
