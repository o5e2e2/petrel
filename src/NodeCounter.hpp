#ifndef NODE_COUNTER_HPP
#define NODE_COUNTER_HPP

#include "typedefs.hpp"
#include "NodeControl.hpp"

class SearchControl;

class NodeCounter {
    node_count_t nodes;
    node_count_t nodesLimit; //search limit

    typedef unsigned nodes_quota_t;
    enum : nodes_quota_t { TickLimit = 5000 }; // ~1 msec
    nodes_quota_t nodesQuota; //number of remaining nodes before slow checking for terminate

public:
    NodeCounter(node_count_t limitNodes = NodeCountMax) {
        nodesLimit = limitNodes;
        nodes = 0;
        nodesQuota = 0;
    }

    NodeControl count(const SearchControl& search);

    node_count_t getNodesVisited() const {
        assert (nodes >= nodesQuota);
        return nodes - nodesQuota;
    }

    void stop() {
        nodesLimit = nodes;
        nodesQuota = 0;
    }

    NodeControl tick() {
        if (nodesQuota > 0) {
            --nodesQuota;
            return NodeControl::Continue;
        }
        return NodeControl::Abort;
    }

    NodeControl refreshQuota() {
        static_assert (TickLimit > 0, "TickLimit should be a positive number");

        assert (nodes >= nodesQuota);
        nodes -= nodesQuota;

        assert (nodesLimit >= nodes);
        auto nodesRemaining = nodesLimit - nodes;

        if (nodesRemaining >= TickLimit) {
            nodesQuota = TickLimit;
        }
        else {
            nodesQuota = static_cast<decltype(nodesQuota)>(nodesRemaining);
            if (nodesQuota == 0) {
                return NodeControl::Abort;
            }
        }

        assert (nodesQuota > 0);
        nodes += nodesQuota;

        --nodesQuota; //count current node
        return NodeControl::Continue;
    }

};

#endif
