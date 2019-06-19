#ifndef UCI_SEARCH_INFO_HPP
#define UCI_SEARCH_INFO_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "SpinLock.hpp"
#include "TimePoint.hpp"

class HashInfo;
class Move;
class PerftTT;
class PositionFen;

class UciSearchInfo {
    io::ostream& out; //output stream
    const PositionFen& positionFen; //current position

    TimePoint fromSearchStart;

    mutable SpinLock outLock;
    mutable bool isreadyWaiting = false; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes = 0;

    void write(io::ostream&, const Move&) const;
    void nps(io::ostream&, node_count_t, const PerftTT&) const;
    void info_nps(io::ostream&, node_count_t, const PerftTT&) const;

public:
    UciSearchInfo (io::ostream&, const PositionFen&);

    //called from Uci
    void isready(bool) const;
    void position() const;
    void uciok(const HashInfo&) const;

    //called from Search
    void clear();

    void readyok(node_count_t, const PerftTT&) const;
    void bestmove(const Move&, Score, node_count_t, const PerftTT&) const;

    void report_perft_divide(const Move&, const Move&, Score, index_t, node_count_t, node_count_t, const PerftTT&) const;
    void report_perft_depth(depth_t, const Move&, Score, node_count_t, node_count_t, const PerftTT&) const;

};

#endif
