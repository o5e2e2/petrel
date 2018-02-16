#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"
#include "TimePoint.hpp"
#include "Move.hpp"

class SearchThread;

enum { PerftNodes, PerftDivideNodes, TT_Tried, TT_Hit, TT_Written, _Total };

class SearchInfo {
protected:
    typedef ::Index<_Total> Index;
    Index::array<node_count_t> _v;

    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    enum { TickLimit = 5000 }; // ~1 msec
    signed nodesQuota; //number of remaining nodes before checking for terminate, normally should never be negative

    TimePoint fromSearchStart;
    depth_t depth; //current search depth

    Move _bestmove;
    Move currmove;
    index_t currmovenumber;

public:
    SearchInfo () { clear(); }

    node_count_t getNodes() const {
        assert (nodesQuota < 0 || nodes >= static_cast<unsigned>(nodesQuota));
        return nodes - static_cast<node_count_t>(nodesQuota);
    }

    void setNodesLimit(node_count_t n) { nodesLimit = n; }

    bool refreshQuota(const SearchThread&);

    bool countNode(const SearchThread& searchThread) {
        if (nodesQuota > 0) {
            --nodesQuota;
            return false;
        }
        return refreshQuota(searchThread);
    }

    void clear();

    void report_perft_divide(Move, index_t, node_count_t);
    void report_perft_depth(depth_t, node_count_t);

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
