#ifndef NODE_COUNTER_HPP
#define NODE_COUNTER_HPP

#include "typedefs.hpp"
#include "NodeControl.hpp"

class SearchControl;

class NodeCounter {
    node_count_t nodes; // (0 <= nodes && nodes <= nodesLimit)
    node_count_t nodesLimit; // search limit

    typedef unsigned nodes_quota_t;
    enum : nodes_quota_t { TickLimit = 5000 }; // ~1 msec

    //number of remaining nodes before slow checking for search abort
    nodes_quota_t nodesQuota; // (0 <= nodesQuota && nodesQuota <= TickLimit)

public:
    NodeCounter(node_count_t limitNodes = NodeCountMax) {
        nodesLimit = limitNodes;
        nodes = 0;
        nodesQuota = 0;
    }

    operator node_count_t () const {
        assert (nodesQuota <= nodes && nodes <= nodesLimit);
        return nodes - nodesQuota;
    }

    bool isAborted() const {
        return nodesLimit == nodes && nodesQuota == 0;
    }

    void abort() {
        nodesLimit = nodes;
        nodesQuota = 0;
    }

    NodeControl count(const SearchControl& search) {
        assert (nodesQuota <= nodes && nodes <= nodesLimit);

        if (nodesQuota > 0) {
            --nodesQuota;
            return NodeControl::Continue;
        }
        return refreshQuota(search);
    }

    NodeControl refreshQuota(const SearchControl& search);

};

#endif
