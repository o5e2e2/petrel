#ifndef UCI_OUTPUT_HPP
#define UCI_OUTPUT_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "HashMemory.hpp"
#include "SearchInfo.hpp"
#include "SpinLock.hpp"

class Move;
class UciPosition;

class UciOutput : public SearchInfo {
    io::ostream& out; //output stream
    io::ostream& err; //error output stream
    const UciPosition& pos;

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(io::ostream&, const Move&) const;
    void hashfull(io::ostream&) const;
    void nps(io::ostream&) const;
    void info_nps(io::ostream&) const;

public:
    UciOutput (const UciPosition&, io::ostream&, io::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory::Info&) const;
    void info_fen() const;

    void error(io::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void readyok() const override;
    void bestmove() const override;
    void info_depth() const override;
    void info_currmove() const override;

};

#endif
