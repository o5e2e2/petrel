#include "SearchLimit.hpp"

namespace {
    std::istream& operator >> (std::istream& in, duration_t& duration) {
        long milliseconds;
        if (in >> milliseconds) {
            duration = std::chrono::duration_cast<duration_t>(std::chrono::milliseconds{milliseconds});
        }
        return in;
    }
}

void SearchLimit::clear() {
    ponder = false;
    time = duration_t::zero();
    op_time = duration_t::zero();
    inc = duration_t::zero();
    op_inc = duration_t::zero();
    movestogo = 0;
    depth = 0;
    nodes = 0;
    mate = 0;
    movetime = duration_t::zero();
    infinite = false;

    perft = true;
    divide = false;
}

std::istream& SearchLimit::read(std::istream& command, const Position& pos, Color color) {
    clear();

    PositionMoves p(pos);
    searchmoves = p.getMoves();

    while (command) {
        if      (command == "depth")    { command >> depth; }
        else if (command == "wtime")    { command >> ((color == White)? time:op_time); }
        else if (command == "btime")    { command >> ((color == Black)? time:op_time); }
        else if (command == "winc")     { command >> ((color == White)? inc:op_inc); }
        else if (command == "binc")     { command >> ((color == Black)? inc:op_inc); }
        else if (command == "movestogo"){ command >> movestogo; }
        else if (command == "nodes")    { command >> nodes; }
        else if (command == "movetime") { command >> movetime; }
        else if (command == "ponder")   { ponder = true; }
        else if (command == "infinite") { infinite = true; }
        else if (command == "searchmoves") { p.limitMoves(command, searchmoves, color); }
        else if (command == "perft")    { perft = true; }
        else if (command == "divide")   { divide = true; }
        else { break; }
    }
    return command;
}

duration_t SearchLimit::getThinkingTime() const {
    if (movetime != duration_t::zero()) { return movetime; }

    index_t moves_to_go{movestogo? movestogo:60};

    duration_t average{(time + moves_to_go*inc) / moves_to_go};
    return std::min(time, average);
}
