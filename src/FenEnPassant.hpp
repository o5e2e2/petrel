#ifndef FEN_EN_PASSANT_HPP
#define FEN_EN_PASSANT_HPP

#include "io.hpp"
#include "typedefs.hpp"

class Position;
class PositionSide;

struct FenEnPassant {
    static std::istream& read(std::istream&, Position&, Color);
    static std::ostream& write(std::ostream&, const PositionSide&, Color);
};

#endif
