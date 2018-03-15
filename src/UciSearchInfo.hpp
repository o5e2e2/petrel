#ifndef UCI_SEARCH_INFO_HPP
#define UCI_SEARCH_INFO_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "Move.hpp"
#include "SpinLock.hpp"
#include "TimePoint.hpp"

class PositionFen;
class SearchControl;
class HashMemory;

class UciSearchInfo {
    io::ostream& out; //output stream
    io::ostream& err; //error output stream
    const PositionFen& pos;

    TimePoint fromSearchStart;

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(io::ostream&, const Move&) const;
    void nps(io::ostream&, node_count_t, const HashMemory&) const;
    void info_nps(io::ostream&, node_count_t, const HashMemory&) const;

public:
    UciSearchInfo (const PositionFen&, io::ostream&, io::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory&) const;
    void info_fen() const;

    void error(io::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void clear();

    void readyok(node_count_t, const HashMemory&) const;
    void bestmove(Move, node_count_t, const HashMemory&) const;

    void report_perft_divide(Move, index_t, node_count_t, node_count_t, const HashMemory&) const;
    void report_perft_depth(depth_t, node_count_t, node_count_t, const HashMemory&) const;

};

#endif
