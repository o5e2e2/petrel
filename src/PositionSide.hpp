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
    Evaluation evaluation;

    void clear(PieceType, Square);
    void drop(PieceType, Square);
    void move(PieceType, Square, Square);

    bool setCastling(Pi);

public:
    PositionSide ();
    PositionSide (const PositionSide&) = delete;
    PositionSide& operator = (const PositionSide&) = default;

    static void swap(PositionSide&, PositionSide&);

    Zobrist getZobrist() const; //calculate Zobrist key

    #ifdef NDEBUG
        void assertValid(Pi) const {}
    #else
        void assertValid(Pi) const;
    #endif

    const Bb& occupiedSquares() const { return piecesBb; }
    bool isOccupied(Square sq) const { return piecesBb[sq]; }
    VectorPiMask alivePieces() const { assert (squares.alivePieces() == types.alivePieces()); return squares.alivePieces(); }

    const MatrixPiBb& allAttacks() const { return attacks; }

    Square squareOf(Pi pi) const { assertValid(pi); return squares.squareOf(pi); }
    Square kingSquare() const { return squareOf(TheKing); }

    bool isPieceOn(Square sq) const { assert (piecesBb[sq] == squares.isPieceOn(sq)); return squares.isPieceOn(sq); }
    Pi pieceOn(Square sq) const { Pi pi = squares.pieceOn(sq); assertValid(pi); return pi; }
    VectorPiMask piecesOn(Square sq) const { return squares.piecesOn(sq); }
    VectorPiMask piecesOn(Rank rank) const { return squares.piecesOn(rank); }

    bool is(Pi pi, PieceType ty) const { assertValid(pi); return types.is(pi, ty); }
    VectorPiMask piecesOfType(PieceType ty) const { return types.piecesOfType(ty); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return types.typeOf(pi); }
    PieceType typeOf(Square sq) const { return typeOf(pieceOn(sq)); }

    const Bb& occupiedByPawns() const { return pawnsBb; }
    VectorPiMask pawns() const { return types.piecesOfType(Pawn); }
    bool isPawn(Pi pi) const { assertValid(pi); return types.isPawn(pi); }

    VectorPiMask sliders() const { return types.sliders(); }
    bool isSlider(Pi pi) const { assertValid(pi); return types.isSlider(pi); }

    VectorPiMask castlingRooks() const { return types.castlingRooks(); }
    bool isCastling(Pi pi) const { assertValid(pi); return types.isCastling(pi); }
    void castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo);

    VectorPiMask enPassantPawns() const { return types.enPassantPawns(); }
    bool hasEnPassant() const { return enPassantPawns().any(); }
    Square enPassantSquare() const { Square ep = squareOf(types.getEnPassant()); assert (ep.is(Rank4)); return ep; }
    File   enPassantFile()   const { return File{ enPassantSquare() }; }
    void markEnPassant(Pi);
    void unmarkEnPassants();
    void setEnPassant(Pi);
    void clearEnPassant();

    const Bb& pinRayFrom(Square) const;
    VectorPiMask pinnerCandidates() const { return types.pinnerCandidates(); }
    void updatePinRays(Square);
    void updatePinRays(Square, Pi);

    void setLeaperAttack(Pi, PieceType, Square);
    void updateSliderAttacks(VectorPiMask, Bb occupied);
    VectorPiMask attacksTo(Square a) const { return attacks[a]; }
    VectorPiMask attacksTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    VectorPiMask attacksTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }

    void capture(Square);
    void move(Pi, PieceType, Square, Square);
    void movePawn(Pi, Square, Square);
    void moveKing(Square, Square);
    void promote(Pi, PromoType, Square, Square);

    //used only during initial position setup
    void drop(Pi, PieceType, Square);
    bool setCastling(File);
    bool setCastling(CastlingSide);

};

#endif
