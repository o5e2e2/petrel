#ifndef UCI_CONTROL_HPP
#define UCI_CONTROL_HPP

#include "io.hpp"
#include "SearchControl.hpp"
#include "UciOutput.hpp"

class UciPosition;

class UciControl {
    UciOutput uciOutput;
    SearchControl searchControl;

public:
    UciControl (const UciPosition&, io::ostream&, io::ostream&);

    void go(io::istream&, const UciPosition&);
    void stop() { searchControl.stop(); }
    void newGame() { searchControl.newGame(); }
    void resizeHash(size_t bytes) { searchControl.tt().resize(bytes); }

    bool isReady() const { return searchControl.isReady(); }

    void error(io::istream& command) const { uciOutput.error(command); }
    void info_fen() const { uciOutput.info_fen(); }
    void uciok()    const { uciOutput.uciok( searchControl.tt().getInfo() ); }
    void readyok()  const { uciOutput.isready( this->isReady() ); }
};

#endif
