#ifndef UCI_POSITION_HPP
#define UCI_POSITION_HPP

#include "io.hpp"
#include "typedefs.hpp"
#include "PositionMoves.hpp"

class UciPosition : public PositionMoves {
    friend class WriteFenBoard;
    friend class WriteFenCastling;

    Color colorToMove; //root position color for moves long algebraic format output
    ChessVariant chessVariant; //format of castling moves output

    Move readMove(std::istream&) const;
    void fenEnPassant(std::ostream&) const;

    void setBoard(std::istream&);
    std::istream& setCastling(std::istream&);
    std::istream& setEnPassant(std::istream&);
    void setupFromFen(std::istream&);

public:
    UciPosition() : PositionMoves(0), colorToMove{White}, chessVariant{Orthodox} {}

    ChessVariant getVariant() const { return chessVariant; }
    Color getColorToMove() const { return colorToMove; }

    void setVariant(ChessVariant v) { chessVariant = v; }

    void playMoves(std::istream&);
    void limitMoves(std::istream&);

    friend std::istream& operator >> (std::istream&, UciPosition&);
    friend std::ostream& operator << (std::ostream&, const UciPosition&);
};

#endif
