#include "Node.hpp"
#include "Position.hpp"
#include "PositionMoves.hpp"
#include "SearchControl.hpp"

#define CUT(found) { if (found) { return true; } } ((void)0)

Node::Node (SearchControl& c, depth_t depth)
    : control(c)
    , draft{depth}
    {}

Node::Node (const Node& parent)
    : control(parent.control)
    , draft{parent.draft-1}
    { --control.info.nodesRemaining; }

bool NodePerft::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    if (draft <= 0) {
        --control.info.nodesRemaining;
        control.info.perftNodes += moves.count();
        return false;
    }

    CUT ( control.checkQuota() );

    NodePerft child{*this};

    for (Pi pi : p.side(My).alive()) {
        Square from{ p.side(My).squareOf(pi) };

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};
            CUT (child(pos));
        }
    }

    return false;
}

bool NodePerftDivide::operator() (const Position& parent) {
    PositionMoves p(parent);
    MatrixPiBb& moves = p.getMoves();

    control.info.currmovenumber = 0;

    NodePerft child{*this};

    for (Pi pi : p.side(My).alive()) {
        Square from = p.side(My).squareOf(pi);

        for (Square to : moves[pi]) {
            moves.clear(pi, to);

            Position pos{parent, from, to};

            auto perftTotal = control.info.perftNodes;
            control.info.perftNodes = 0;

            CUT (child(pos));

            control.info.currmovenumber++;
            control.info.currmove = parent.createMove(My, from, to);
            control.report_perft_divide();

            control.info.perftNodes += perftTotal;
        }
    }

    control.report_perft_depth();
    return false;
}

bool NodePerftRoot::perft(const Position& parent) {
    control.info.perftNodes = 0;
    control.info.depth = draft;

    bool isAborted = NodePerft::operator()(parent);

    if (!isAborted) {
        control.report_perft_depth();
    }

    return isAborted;
}

bool NodePerftRoot::operator() (const Position& parent) {
    if (draft > 0) {
        perft(parent);
    }
    else {
        for (draft = 1; !control.isStopped(); ++draft) {
            perft(parent);
        }
    }

    control.report_bestmove();
    return true;
}

bool NodePerftDivideRoot::operator() (const Position& parent) {
    if (draft > 0) {
        control.info.depth = draft;
        NodePerftDivide::operator()(parent);
    }
    else {
        for (draft = 1; !control.isStopped(); ++draft) {
            control.info.perftNodes = 0;
            control.info.depth = draft;
            NodePerftDivide::operator()(parent);
        }
    }

    control.report_bestmove();
    return true;
}
