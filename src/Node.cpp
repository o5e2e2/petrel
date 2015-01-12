#include "Node.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

Node::Node (SearchControl& c, depth_t depth)
    : control(c)
    , draft{depth}
    , nodesRemaining{0}
    {}

Node::Node (const Node& parent)
    : control(parent.control)
    , draft{parent.draft-1}
    , nodesRemaining{parent.nodesRemaining-1}
    {}

bool Node::checkQuota() {
    if (nodesRemaining <= 0) {
        control.acquireNodesQuota(nodesRemaining);
    }
    return nodesRemaining <= 0;
}

bool NodePerft::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    if (draft <= 0) {
        --nodesRemaining;
        control.info.perftNodes += moves.count();
        return false;
    }

    CUT ( checkQuota() );

    NodePerft child{*this};

    for (Pi pi : p.side(My).alive()) {
        Square from{ p.side(My).squareOf(pi) };

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};
            CUT (child(pos));
        }
    }

    nodesRemaining = child.nodesRemaining;
    return false;
}

bool NodePerftDivide::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    index_t currmovenumber = 0;

    NodePerft child{*this};

    for (Pi pi : p.side(My).alive()) {
        Square from = p.side(My).squareOf(pi);

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};

            auto perftTotal = control.info.perftNodes;
            control.info.perftNodes = 0;

            CUT (child(pos));

            control.report_perft_divide(child.nodesRemaining, parent.createMove(My, from, to), ++currmovenumber);
            control.info.perftNodes += perftTotal;
        }
    }
    nodesRemaining = child.nodesRemaining;
    return false;
}

bool NodePerftRoot::operator() (const Position& parent) {
    if (draft > 0) {
        if (! NodePerft::operator()(parent) ) {
            control.report_perft_depth(nodesRemaining, draft);
        }
    }
    else {
        for (depth_t iteration{1}; !control.isStopped(); ++iteration) {
            control.info.perftNodes = 0;
            draft = iteration;
            if (! NodePerft::operator()(parent) ) {
                control.report_perft_depth(nodesRemaining, iteration);
            }
        }
    }

    control.report_bestmove(nodesRemaining);
    return true;
}

bool NodePerftDivideRoot::operator() (const Position& parent) {
    if (draft > 0) {
        if (! NodePerftDivide::operator()(parent) ) {
            control.report_perft_depth(nodesRemaining, draft);
        }
    }
    else {
        for (depth_t iteration{1}; !control.isStopped(); ++iteration) {
            draft = iteration;
            if (! NodePerftDivide::operator()(parent) ) {
                control.report_perft_depth(nodesRemaining, iteration);
            }
        }
    }

    control.report_bestmove(nodesRemaining);
    return true;
}
