#ifndef NODE_COUNTER_HPP
#define NODE_COUNTER_HPP

#include "typedefs.hpp"
#include "Node.hpp"

class SearchControl;

class NodeCounter {
    node_count_t nodes = 0; // (0 <= nodes && nodes <= nodesLimit)
    node_count_t nodesLimit; // search limit

    typedef unsigned nodes_quota_t;
    enum : nodes_quota_t { QuotaLimit = 5000 }; // ~1 msec

    //number of remaining nodes before slow checking for search abort
    nodes_quota_t nodesQuota = 0; // (0 <= nodesQuota && nodesQuota <= QuotaLimit)

    constexpr void assertValid() const {
        assert (nodesQuota <= nodes && nodes <= nodesLimit);
        assert (/* 0 <= nodesQuota && */ nodesQuota < QuotaLimit);
    }

public:
    constexpr NodeCounter(node_count_t n = NodeCountMax) : nodesLimit{n} {}

    /// exact number of visited nodes
    constexpr operator node_count_t () const {
        assertValid();
        return nodes - nodesQuota;
    }

    constexpr bool isAborted() const {
        assertValid();
        assert (nodes - nodesQuota < nodesLimit || nodesQuota == 0);
        return nodes == nodesLimit;
    }

    NodeControl count(const SearchControl& search) {
        assertValid();

        if (nodesQuota == 0) {
            return refreshQuota(search);
        }

        assert (nodesQuota > 0);
        --nodesQuota;

        assertValid();
        return NodeControl::Continue;
    }

    NodeControl refreshQuota(const SearchControl& search);

};

#endif
