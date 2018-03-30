#ifndef POSITION_HPP
#define POSITION_HPP

#include "PositionSide.hpp"
#include "Zobrist.hpp"

class Position {
protected:
    Side::array<PositionSide> side;
    Zobrist zobrist{0};

private:
    template <Side::_t> void updateSliderAttacks(VectorPiMask);
    template <Side::_t> void updateSliderAttacks(VectorPiMask, VectorPiMask);
    template <Side::_t> void setLegalEnPassant(Pi);
    template <Side::_t> void capture(Square);
    template <Side::_t> void setGamePhase();
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
    void playMove(const Position&, Square, Square);
    void playMove(Square, Square);

    //initial position setup
    bool drop(Side, PieceType, Square);
    bool setCastling(Side, File);
    bool setCastling(Side, CastlingSide);
    bool setEnPassant(File);
    bool setup();
};

#endif
