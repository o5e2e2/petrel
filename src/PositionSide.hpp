#ifndef POSITION_SIDE_HPP
#define POSITION_SIDE_HPP

#include "Evaluation.hpp"
#include "MatrixPiBb.hpp"
#include "Move.hpp"
#include "VectorPiType.hpp"
#include "VectorPiSquare.hpp"
#include "Zobrist.hpp"

/******************************************************************************
 * static information about pieces of the single player's side (either on move or opponent)
 */

//TRICK: all squares are relative to its own side (so the king piece is initially on E1 square regardless color)
class PositionSide {
    MatrixPiBb attacks; //squares attacked by a piece and pieces attacking to a square
    VectorPiType types; //type of each alive piece, rooks with castling rights, pawns affected by en passant
    VectorPiSquare squares; //onboard square locations of the alive pieces or 'NoSquare' special value

    Bb occupiedBb; //all occupied squares by both sides, updated by combining piecesBb of both sides
    Bb piecesBb; //all pieces of the current side, incrementally updated
    Bb pawnsBb; //pawns of the current side, incrementally updated

    Square opKing; //location of the opponent's king

    Evaluation evaluation;

    void move(Pi, PieceType, Square, Square);
    void setLeaperAttack(Pi, PieceType, Square);
    void updatePinner(Pi, Square);
    GamePhase generateGamePhase() const; //returns whether material for endgame or middlegame

friend class Position;
    static void swap(PositionSide&, PositionSide&);
    static void finalSetup(PositionSide&, PositionSide&);
    static void syncOccupied(PositionSide&, PositionSide&);

    void setOpKing(Square);
    void move(Pi, Square, Square);
    void movePawn(Pi, Square, Square);
    void moveKing(Square, Square);
    void castle(Square kingFrom, Square kingTo, Pi rook, Square rookFrom, Square rookTo);
    void promote(Pi, PromoType, Square, Square);
    void capture(Square);

    void setEnPassantVictim(Pi);
    void setEnPassantKiller(Pi);
    void clearEnPassantVictim();
    void clearEnPassantKillers();

    void setSliderAttacks(VectorPiMask, Bb);
    void setGamePhase(const PositionSide&);

    //used only during initial position setup
    bool dropValid(PieceType, Square);
    bool setValidCastling(File);
    bool setValidCastling(CastlingSide);

public:
    constexpr PositionSide () = default;
    PositionSide (const PositionSide&) = default;

    static Score evaluate(const PositionSide&, const PositionSide&);

    Zobrist generateZobrist() const; //calculate Zobrist key from scratch

    #ifdef NDEBUG
        void assertValid(Pi) const {}
        void assertValid(Pi, PieceType, Square) const {}
    #else
        void assertValid(Pi) const;
        void assertValid(Pi, PieceType, Square) const;
    #endif

    const Bb& occupied() const { return occupiedBb; }
    const Bb& sideSquares() const { return piecesBb; }
    bool isOccupied(Square sq) const { return piecesBb.has(sq); }
    VectorPiMask alivePieces() const { assert (squares.alivePieces() == types.alivePieces()); return squares.alivePieces(); }

    Square squareOf(Pi pi) const { assertValid(pi); return squares.squareOf(pi); }
    Square kingSquare() const { return squareOf(TheKing); }
    const Square& opKingSquare() const { return opKing; }

    bool hasPieceOn(Square sq) const { assert (piecesBb.has(sq) == squares.hasPieceOn(sq)); return squares.hasPieceOn(sq); }
    Pi pieceOn(Square sq) const { Pi pi = squares.pieceOn(sq); assertValid(pi); return pi; }
    VectorPiMask piecesOn(Square sq) const { return squares.piecesOn(sq); }
    VectorPiMask piecesOn(Rank rank) const { return squares.piecesOn(rank); }

    bool isOfType(Pi pi, PieceType ty) const { assertValid(pi); return types.isOfType(pi, ty); }
    VectorPiMask piecesOfType(PieceType ty) const { return types.piecesOfType(ty); }
    PieceType typeOf(Pi pi) const { assertValid(pi); return types.typeOf(pi); }
    PieceType typeOf(Square sq) const { return typeOf(pieceOn(sq)); }

    const Bb& pawnsSquares() const { return pawnsBb; }
    VectorPiMask pawns() const { return types.piecesOfType(Pawn); }
    bool isPawn(Pi pi) const { assertValid(pi); return types.isPawn(pi); }

    VectorPiMask sliders() const { return types.sliders(); }
    bool isSlider(Pi pi) const { assertValid(pi); return types.isSlider(pi); }

    VectorPiMask castlingRooks() const { return types.castlingRooks(); }
    bool isCastling(Pi pi) const { assertValid(pi); return types.isCastling(pi); }
    bool isCastling(Square sq) const { return isCastling(pieceOn(sq)); }

    VectorPiMask enPassantPawns() const { return types.enPassantPawns(); }
    bool hasEnPassant() const { return enPassantPawns().any(); }
    Square enPassantSquare() const { Square ep = squareOf(types.getEnPassant()); assert (ep.on(Rank4)); return ep; }
    File   enPassantFile()   const { return File{ enPassantSquare() }; }

    VectorPiMask pinners() const { return types.pinners(); }
    bool isPinned(Bb) const;

    const MatrixPiBb& attacksMatrix() const { return attacks; }
    VectorPiMask attackersTo(Square a) const { return attacks[a]; }
    VectorPiMask attackersTo(Square a, Square b) const { return attacks[a] | attacks[b]; }
    VectorPiMask attackersTo(Square a, Square b, Square c) const { return attacks[a] | attacks[b] | attacks[c]; }
    VectorPiMask checkers() const { return attacks[opKing]; }

    Move createMove(Square from, Square to) const;

};

#endif
