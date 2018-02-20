#ifndef UCI_SEARCH_INFO_HPP
#define UCI_SEARCH_INFO_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "HashMemory.hpp"
#include "Move.hpp"
#include "SearchInfo.hpp"
#include "SpinLock.hpp"
#include "TimePoint.hpp"

class PositionFen;

class UciSearchInfo : public SearchInfo {
    io::ostream& out; //output stream
    io::ostream& err; //error output stream
    const PositionFen& pos;

    TimePoint fromSearchStart;
    depth_t depth; //current search depth
    Move _bestmove;
    Move currmove;
    index_t currmovenumber;

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(io::ostream&, const Move&) const;
    void hashfull(io::ostream&) const;
    void nps(io::ostream&) const;
    void info_nps(io::ostream&) const;

public:
    UciSearchInfo (const PositionFen&, io::ostream&, io::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory::Info&) const;
    void info_fen() const;

    void error(io::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void searchStarted() override;
    void readyok() const override;
    void bestmove() const override;
    void info_depth() const override;
    void info_currmove() const override;

    void report_perft_divide(Move, index_t, node_count_t);
    void report_perft_depth(depth_t, node_count_t);

};

#endif
