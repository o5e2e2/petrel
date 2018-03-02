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

    enum : unsigned { TickLimit = 5000 }; // ~1 msec
    unsigned nodesQuota; //number of remaining nodes before slow checking for terminate

    bool refreshQuota(const SearchControl&);

public:
    SearchInfo () { clear(); }
    void clear();

    node_count_t getNodes() const {
        assert (nodes >= nodesQuota);
        return nodes - nodesQuota;
    }

    void setNodesLimit(node_count_t n) { nodesLimit = n; nodes = 0; nodesQuota = 0; }

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
    virtual ~SearchInfo() = default;
};

#endif
