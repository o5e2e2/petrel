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

    Bb occupiedBb; //all occupied squares by both sides, updated from piecesBb of both sides
    Bb piecesBb; //all pieces of the current side, incrementally updated
    Bb pawnsBb; //pawns of the current side, incrementally updated

    Evaluation evaluation;

    void clear(PieceType, Square);
    void drop(PieceType, Square);
    void move(PieceType, Square, Square);
    bool setCastling(Pi);
    void setLeaperAttack(Pi, PieceType, Square);

friend class Position;
    static void swap(PositionSide&, PositionSide&);

    void move(Pi, PieceType, Square, Square);
    void movePawn(Pi, Square, Square);
    void moveKing(Square, Square);
    void castle(Pi rook, Square rookFrom, Square rookTo, Square kingFrom, Square kingTo);
    void promote(Pi, PromoType, Square, Square);
    void capture(Square);

    //set/clear for just played pawn jump, mark/unmark for legal attacker to ep pawn
    void setEnPassant(Pi);
    void markEnPassant(Pi);
    void clearEnPassant();
    void unmarkEnPassants();

    void setOccupied(const Bb& occupied) { occupiedBb = occupied; }
    void setSliderAttacks(VectorPiMask, Bb occupied);
    void setStage(EvalStage opStage) { evaluation.setStage(opStage, kingSquare()); }

    //used only during initial position setup
    void drop(Pi, PieceType, Square);
    bool setCastling(File);
    bool setCastling(CastlingSide);

public:
    PositionSide () {};
    PositionSide (int);
    PositionSide (const PositionSide&) = default;

    Zobrist generateZobrist() const; //calculate Zobrist key from scratch

    #ifdef NDEBUG
        void assertValid(Pi) const {}
    #else
        void assertValid(Pi) const;
    #endif

    const Bb& occupied() const { return occupiedBb; }
    const Bb& occupiedSquares() const { return piecesBb; }
    bool isOccupied(Square sq) const { return piecesBb[sq]; }
    VectorPiMask alivePieces() const { assert (squares.alivePieces() == types.alivePieces()); return squares.alivePieces(); }

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

    VectorPiMask enPassantPawns() const { return types.enPassantPawns(); }
    bool hasEnPassant() const { return enPassantPawns().any(); }
    Square enPassantSquare() const { Square ep = squareOf(types.getEnPassant()); assert (ep.is(Rank4)); return ep; }
    File   enPassantFile()   const { return File{ enPassantSquare() }; }

    const Bb& pinRayFrom(Square) const;
    bool canBeAttacked(Square from, Square to) const;

    const MatrixPiBb& allAttacks() const { return attacks; }
    VectorPiMask attacksTo(Square a) const { return attacks[a]; }
    VectorPiMask attacksTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    VectorPiMask attacksTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }

    EvalStage getStage() const; //returns whether material for endgame or middlegame

};

#endif
