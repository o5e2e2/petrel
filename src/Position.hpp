#ifndef POSITION_HPP
#define POSITION_HPP

#include "io.hpp"
#include "PositionSide.hpp"
#include "Zobrist.hpp"
#include "Move.hpp"

class Position {

protected:
    Side::array<PositionSide> side;
    Zobrist zobrist;

private:
    template <Side::_t> bool isPinned(Square, Bb, Bb) const;
    template <Side::_t> void movePiece(Pi, Square, Square);
    template <Side::_t> void movePawn(Pi, Square, Square);
    template <Side::_t> void setSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacksKing(VectorPiMask); //remove king to avoid hiding it under its own shadow when in check
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void capture(Square);
    template <Side::_t> void setStage(); //recalculate game stage for position evaluation

    template <Side::_t> void playKingMove(Square, Square);
    template <Side::_t> void playPawnMove(Pi, Square, Square);
    template <Side::_t> void playCastling(Pi, Square, Square);
    template <Side::_t> void playMove(Square, Square);

    Color setBoard(std::istream&);

    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    std::istream& setCastling(std::istream&, Color);

    bool setEnPassant(File);
    std::istream& setEnPassant(std::istream&, Color);

public:
    Position (const Position&) = default;
    Position (int) {};

    VectorPiMask alivePieces() const { return side[My].alivePieces(); }
    Square squareOf(Pi pi) const { return side[My].squareOf(pi); }

    Zobrist generateZobrist() const;
    Zobrist makeZobrist(Square from, Square to) const;

    void playMove(const Position&, Square, Square, Zobrist = {});
    Move createMove(Square, Square) const;

    //initial position setup
    Color setupFromFen(std::istream&);
    bool drop(Side, PieceType, Square);
    bool setup();
    void playMove(Square, Square);

};

#endif
