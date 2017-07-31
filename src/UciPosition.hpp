#ifndef UCI_POSITION_HPP
#define UCI_POSITION_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"

class UciPosition : public PositionMoves {
    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output
    Move readMove(std::istream& in) const;

public:
    UciPosition() : PositionMoves(0), colorToMove{White}, chessVariant{Orthodox} {}
    void setVariant(ChessVariant v) { chessVariant = v; }
    ChessVariant getVariant() const { return chessVariant; }

    Color getColorToMove() const { return colorToMove; }

    friend std::ostream& operator << (std::ostream& out, const UciPosition& pos) {
        pos.fen(out, pos.colorToMove, pos.chessVariant);
        return out;
    }

    void setFen(std::istream&);
    void makeMoves(std::istream&);
    void limitMoves(std::istream&);
};

#endif
