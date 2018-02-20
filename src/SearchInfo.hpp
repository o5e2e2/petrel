#ifndef SEARCH_INFO_HPP
#define SEARCH_INFO_HPP

#include "typedefs.hpp"

class SearchControl;

enum { PerftNodes, PerftDivideNodes, TT_Tried, TT_Hit, TT_Written, _TotalCounters };

class SearchInfo {
protected:
    typedef ::Index<_TotalCounters> Index;
    Index::array<node_count_t> _v;

    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    enum { TickLimit = 5000 }; // ~1 msec
    signed nodesQuota; //number of remaining nodes before checking for terminate, normally should never be negative

    bool refreshQuota(const SearchControl&);

public:
    SearchInfo () { clear(); }
    void clear();

    node_count_t getNodes() const {
        assert (nodesQuota < 0 || nodes >= static_cast<unsigned>(nodesQuota));
        return nodes - static_cast<unsigned>(nodesQuota);
    }

    void setNodesLimit(node_count_t n) { nodesLimit = n; }

    bool countNode(const SearchControl& searchControl) {
        if (nodesQuota > 0) {
            --nodesQuota;
            return false;
        }
        return refreshQuota(searchControl);
    }

    node_count_t get(Index i) const { return _v[i]; }
    void inc(Index i, node_count_t n = 1) { _v[i] += n; }

    virtual void searchStarted() { clear(); };

    //do nothing by default
    virtual void bestmove() const {};
    virtual void readyok() const {};
    virtual void info_depth() const {};
    virtual void info_currmove() const {};
    virtual ~SearchInfo() {}
};

#endif
