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
    template <Side::_t> void move(Pi, Square, Square);
    template <Side::_t> void movePawn(Pi, Square, Square);
    template <Side::_t> void setSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacksKing(VectorPiMask); //remove king to avoid hiding it under its own shadow when in check
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void makeKingMove(Square, Square);
    template <Side::_t> void makePawnMove(Pi, Square, Square);
    template <Side::_t> void makeCastling(Pi, Square, Square);
    template <Side::_t> void makeMove(Square, Square);
    template <Side::_t> void capture(Square);
    template <Side::_t> void setStage(); //recalculate game stage for position evaluation

    Color setBoard(std::istream&);
    static void fenBoard(std::ostream&, const PositionSide& white, const PositionSide& black);

    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    std::istream& setCastling(std::istream&, Color);

    bool setEnPassant(File);
    std::istream& setEnPassant(std::istream&, Color);
    void fenEnPassant(std::ostream& out, Color colorToMove) const;

public:
    Position (const Position&) = default;
    Position (int) {};

    VectorPiMask alivePieces() const { return side[My].alivePieces(); }
    Square squareOf(Pi pi) const { return side[My].squareOf(pi); }

    Zobrist generateZobrist() const;

    void makeMove(const Position&, Square, Square, Zobrist = {});

    Zobrist makeZobrist(Square from, Square to) const;
    void fen(std::ostream&, Color, ChessVariant) const;
    Move createMove(Square, Square) const;

    //initial position setup
    Color setFen(std::istream&);
    bool drop(Side, PieceType, Square);
    bool setup();
    void makeMove(Square, Square);

};

#endif
