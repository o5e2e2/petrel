#ifndef POSITION_SIDE_HPP
#define POSITION_SIDE_HPP

#include "Evaluation.hpp"
#include "MatrixPiBb.hpp"
#include "VectorPiType.hpp"
#include "VectorPiSquare.hpp"
#include "Zobrist.hpp"

/******************************************************************************
 * static information about pieces of one player's side (either on move or opponent)
 */

//TRICK: all squares are relative to the own side (so king piece is initially on E1 square regardless color)
class PositionSide {
    MatrixPiBb attacks; //squares attacked by a piece and pieces attacking to a square
    VectorPiType types; //type of each alive piece, rooks with castling rights, pawns affected by en passant
    VectorPiSquare squares; //onboard square locations of the alive pieces or 'NoSquare' special value

    Bb piecesBb; //all pieces of the current side
    Bb pawnsBb; //pawns of the current side
    Zobrist zobrist; //Zobrist hash key of pieces of the current side
    Evaluation evaluation;

    void set(PieceType, Square);
    void clear(PieceType, Square);

    void setCastling(Pi);
    void clearCastling(Pi, Square);
    void clearCastlings();
    bool isCastling(Pi pi) const { assertValid(pi); return types.isCastling(pi); }

public:
    PositionSide ();
    PositionSide (const PositionSide&) = delete;
    PositionSide& operator = (const PositionSide&) = default;

    static void swap(PositionSide&, PositionSide&);

    #ifdef NDEBUG
        void assertValid(Pi) const {}
    #else
        void assertValid(Pi) const;
    #endif

    const Bb& occ() const { return piecesBb; }
    const Bb& occPawns() const { return pawnsBb; }
    const MatrixPiBb& allAttacks() const { return attacks; }

    static Zobrist zobrist_combine(const PositionSide& my, const PositionSide& op) { return Zobrist::combine(my.zobrist, op.zobrist); }

    bool operator [] (Square sq) const { return piecesBb[sq]; }

    bool isOn(Square sq) const { assert (piecesBb[sq] == squares.isOn(sq)); return squares.isOn(sq); }
    Square squareOf(Pi pi) const { assertValid(pi); return squares.squareOf(pi); }
    Square kingSquare() const { return squareOf(TheKing); }
    Pi pieceOn(Square sq) const { assert (isOn(sq)); Pi pi = squares.pieceOn(sq); assertValid(pi); return pi; }
    VectorPiMask on(Square sq) const { return squares.on(sq); }
    VectorPiMask of(Rank rank) const { return squares.of(rank); }

    bool is(Pi pi, PieceType ty) const { assertValid(pi); return types.is(pi, ty); }
    VectorPiMask of(PieceType ty) const { return types.of(ty); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return types.typeOf(pi); }
    PieceType typeOf(Square sq) const { return typeOf(pieceOn(sq)); }

    VectorPiMask alive() const { assert (squares.alive() == types.alive()); return squares.alive(); }
    VectorPiMask pawns() const { return types.of(Pawn); }
    VectorPiMask sliders() const { return types.sliders(); }
    bool isSlider(Pi pi) const { assertValid(pi); return types.isSlider(pi); }

    void capture(Square);
    void move(Pi, PieceType, Square, Square);
    void moveKing(Square, Square);
    void promote(Pi, PromoType, Square, Square);

    CastlingSide castlingSideOf(Pi pi) const { assert (isCastling(pi)); return squareOf(pi) < kingSquare()? QueenSide:KingSide; }
    VectorPiMask castlingRooks() const { return types.castlingRooks(); }
    void castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo);

    VectorPiMask enPassantPawns() const { return types.enPassantPawns(); }
    bool hasEnPassant() const { return enPassantPawns().any(); }
    File enPassantFile() const { Square ep = squareOf(types.getEnPassant()); assert (ep.is(Rank4)); return File{ep}; }
    void markEnPassant(Pi);
    void setEnPassant(Pi, File);
    void clearEnPassant();
    void clearEnPassants();

    const Bb& pinRayFrom(Square) const;
    VectorPiMask pinnerCandidates() const { return types.pinnerCandidates(); }
    void updatePinRays(Square);
    void updatePinRays(Square, Pi);

    void setLeaperAttack(Pi, PieceType, Square);
    void updateSliderAttacks(VectorPiMask, Bb occupied);
    VectorPiMask attacksTo(Square a) const { return attacks[a]; }
    VectorPiMask attacksTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    VectorPiMask attacksTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }

    //used only during initial position setup
    void drop(Pi, PieceType, Square);
    bool setCastling(File);
    bool setCastling(CastlingSide);

};

#endif
