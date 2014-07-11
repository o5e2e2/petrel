#include "Node.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

Node::Node (depth_t depth)
    : draft{depth}
    , nodesRemaining{0}
    {}

Node::Node (const Node& parent)
    : draft{parent.draft-1}
    , nodesRemaining{parent.nodesRemaining-1}
    {}

bool Node::checkQuota() {
    if (nodesRemaining <= 0) {
        The_game.acquireNodesQuota(nodesRemaining);
    }
    return nodesRemaining <= 0;
}

bool NodePerft::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    if (draft <= 0) {
        --nodesRemaining;
        perftNodes += p.countLegalMoves();
        return false;
    }

    CUT ( checkQuota() );

    NodePerft child{*this};
    //assert (child.perftNodes == 0);
    child.perftNodes = 0;

    for (Pi pi : p.side(My).alive()) {
        Square from{ p.side(My).squareOf(pi) };

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};
            CUT (child(pos));
        }
    }

    nodesRemaining = child.nodesRemaining;
    perftNodes += child.perftNodes;
    return false;
}

bool NodePerftDivide::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    index_t currmovenumber = 0;

    NodePerft child{*this};
    //assert (child.perftNodes == 0);
    child.perftNodes = 0;

    for (Pi pi : p.side(My).alive()) {
        Square from = p.side(My).squareOf(pi);

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};

            child.perftNodes = 0;
            CUT (child(pos));

            The_game.releaseNodesQuota(child.nodesRemaining);
            The_game.report_perft(createMove(parent, My, from, to), ++currmovenumber, child.perftNodes);

            perftNodes += child.perftNodes;
        }
    }
    nodesRemaining = child.nodesRemaining;
    return false;
}

bool NodePerftRoot::operator() (const Position& parent) {
    if (draft > 0) {
        if (! NodePerft::operator()(parent) ) {
            The_game.releaseNodesQuota(nodesRemaining);
            The_game.report_perft_depth(draft, perftNodes);
        }
    }
    else {
        for (depth_t iteration{1}; !The_game.isStopped(); ++iteration) {
            perftNodes = 0;
            draft = iteration;
            if (! NodePerft::operator()(parent) ) {
                The_game.releaseNodesQuota(nodesRemaining);
                The_game.report_perft_depth(iteration, perftNodes);
            }
        }
    }

    The_game.releaseNodesQuota(nodesRemaining);
    The_game.report_bestmove(Move::null());
    return true;
}

bool NodePerftDivideRoot::operator() (const Position& parent) {
    if (draft > 0) {
        if (! NodePerftDivide::operator()(parent) ) {
            The_game.releaseNodesQuota(nodesRemaining);
            The_game.report_perft_depth(draft, perftNodes);
        }
    }
    else {
        for (depth_t iteration{1}; !The_game.isStopped(); ++iteration) {
            draft = iteration;
            if (! NodePerftDivide::operator()(parent) ) {
                The_game.releaseNodesQuota(nodesRemaining);
                The_game.report_perft_depth(iteration, perftNodes);
            }
        }
    }

    The_game.releaseNodesQuota(nodesRemaining);
    The_game.report_bestmove(Move::null());
    return true;
}
