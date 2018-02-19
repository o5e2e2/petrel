#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"
#include "Zobrist.hpp"

class Position {
protected:
    Side::array<PositionSide> side;
    Zobrist zobrist;

private:
    template <Side::_t> bool isPinned(Bb) const;
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

public:
    constexpr Position () = default;
    Position (const Position&) = default;

    VectorPiMask alivePieces() const { return side[My].alivePieces(); }
    Square squareOf(Pi pi) const { return side[My].squareOf(pi); }

    Zobrist generateZobrist() const;
    Zobrist createZobrist(Square, Square) const;
    Move createMove(Square from, Square to) const { return side[My].createMove(from, to); }

    void playMove(const Position&, Square, Square, Zobrist);
    void playMove(const Position& parent, Square from, Square to) { playMove(parent, from, to, createZobrist(from, to)); }
    void playMove(Square, Square);

    //initial position setup
    void clear();
    bool drop(Side, PieceType, Square);
    bool setCastling(Side si, File file) { return side[si].setCastling(file); }
    bool setCastling(Side si, CastlingSide castlingSide) { return side[si].setCastling(castlingSide); }
    bool setEnPassant(File);
    bool setup();
};

#endif
