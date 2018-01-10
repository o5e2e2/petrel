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
    std::ostream& out; //output stream
    std::ostream& err; //error output stream
    const UciPosition& pos;

    mutable SpinLock outLock;
    mutable volatile bool isreadyWaiting; //set when got 'isready' command while thinking
    mutable node_count_t lastInfoNodes;

    void write(std::ostream&, const Move&) const;
    void hashfull(std::ostream&) const;
    void nps(std::ostream&) const;
    void info_nps(std::ostream&) const;

public:
    UciOutput (const UciPosition&, std::ostream&, std::ostream& = std::cerr);

    //called from Uci
    void isready(bool) const;
    void uciok(const HashMemory::Info&) const;
    void info_fen() const;

    void error(std::istream&) const;
    void error(const std::string&) const;

    //called from Search
    void readyok() const override;
    void bestmove() const override;
    void info_depth() const override;
    void info_currmove() const override;

};

#endif
