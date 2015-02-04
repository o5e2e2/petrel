#ifndef UCI_HPP
#define UCI_HPP

#include <sstream>

#include "io.hpp"
#include "Position.hpp"
#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "UciOutput.hpp"

class Uci {
    SearchControl searchControl;
    UciOutput uciOutput;
    Position rootPosition; //initial chess position to analyze
    SearchLimit goLimit;

    ChessVariant chessVariant; //format of castling moves output
    Color colorToMove; //initial position color for long algebraic format moves output

    mutable std::istringstream command; //current input command line
    bool next(io::literal keyword) const { return io::next(command, keyword); }

    //UCI command handlers
    void uci() const;
    void isready() const;
    void ucinewgame();
    void setoption();
    void position();
    void setStartpos();
    void go();

    //UCI protocol extensions
    void echo() const;
    void call();

public:
    Uci (std::ostream& = std::cout);
    bool operator() (const std::string& filename);
    bool operator() (std::istream& = std::cin);
};

#endif
