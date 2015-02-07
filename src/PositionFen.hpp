#ifndef POSITION_FEN_HPP
#define POSITION_FEN_HPP

#include "io.hpp"
#include "typedefs.hpp"

class Position;

/**
 * Setup a chess position from a FEN string with chess legality validation
 **/
namespace PositionFen {
    void read(std::istream&, Position&, Color&);
    void setStartpos(Position&, Color&);

    void write(std::ostream&, const Position&, Color, ChessVariant);
}

#endif
