#ifndef UCI_SEARCH_INFO_HPP
#define UCI_SEARCH_INFO_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "HashMemory.hpp"
#include "Move.hpp"
#include "SpinLock.hpp"
#include "TimePoint.hpp"

class PositionFen;
class SearchControl;

enum { PerftNodes, PerftDivideNodes, TT_Tried, TT_Hit, TT_Written, _TotalCounters };

class UciSearchInfo {
    typedef ::Index<_TotalCounters> Index;
    Index::array<node_count_t> _v;

    io::ostream& out; //output stream
    io::ostream& err; //error output stream
    const PositionFen& pos;

    TimePoint fromSearchStart;

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(io::ostream&, const Move&) const;
    void hashfull(io::ostream&) const;
    void nps(io::ostream&, node_count_t) const;
    void info_nps(io::ostream&, node_count_t) const;

public:
    UciSearchInfo (const PositionFen&, io::ostream&, io::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory::Info&) const;
    void info_fen() const;

    void error(io::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void clear();

    node_count_t get(Index i) const { return _v[i]; }
    void inc(Index i, node_count_t n = 1) { _v[i] += n; }

    void readyok(node_count_t) const;
    void bestmove(Move, node_count_t) const;

    void report_perft_divide(Move, index_t, node_count_t, node_count_t) const;
    void report_perft_depth(depth_t, node_count_t, node_count_t) const;

};

#endif
