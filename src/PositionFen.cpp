#include <set>

#include "PositionFen.hpp"
#include "FenBoard.hpp"
#include "CastlingRules.hpp"

#define MY (*this)[My]
#define OP (*this)[Op]

class WriteFenBoard {
    const PositionSide& whitePieces;
    const PositionSide& blackPieces;

public:
    WriteFenBoard (const PositionSide& w, const PositionSide& b) :
        whitePieces{w},
        blackPieces{b}
        {}

    friend io::ostream& operator << (io::ostream& out, const WriteFenBoard& fen) {
        FOR_INDEX(Rank, rank) {
            index_t emptySqCount = 0;

            FOR_INDEX(File, file) {
                Square sq(file,rank);

                if (fen.whitePieces.isOccupied(sq)) {
                    if (emptySqCount != 0) { out << emptySqCount; emptySqCount = 0; }
                    out << static_cast<io::char_type>(std::toupper( fen.whitePieces.typeOf(sq).to_char() ));
                    continue;
                }

                if (fen.blackPieces.isOccupied(~sq)) {
                    if (emptySqCount != 0) { out << emptySqCount; emptySqCount = 0; }
                    out << fen.blackPieces.typeOf(~sq).to_char();
                    continue;
                }

                ++emptySqCount;
            }

            if (emptySqCount != 0) { out << emptySqCount; }
            if (rank != Rank1) { out << '/'; }
        }

        return out;
    }
};

class WriteFenCastling {
    std::set<io::char_type> castlingSet;

    void insert(const PositionSide& positionSide, Color color, ChessVariant chessVariant) {
        for (Pi pi : positionSide.castlingRooks()) {
            io::char_type castlingSymbol;

            switch (chessVariant) {
                case Chess960:
                    castlingSymbol = File(positionSide.squareOf(pi)).to_char();
                    break;

                case Orthodox:
                default:
                    castlingSymbol = CastlingRules::castlingSide(positionSide.kingSquare(), positionSide.squareOf(pi)).to_char();
                    break;
            }

            if (color.is(White)) {
                castlingSymbol = static_cast<io::char_type>(std::toupper(castlingSymbol));
            }

            castlingSet.insert(castlingSymbol);
        }
    }

public:
    WriteFenCastling (const PositionSide& whitePieces, const PositionSide& blackPieces, ChessVariant chessVariant) {
        insert(whitePieces, White, chessVariant);
        insert(blackPieces, Black, chessVariant);
    }

    friend io::ostream& operator << (io::ostream& out, const WriteFenCastling& fen) {
        if (fen.castlingSet.empty()) {
            return out << '-';
        }

        for (auto castlingSymbol : fen.castlingSet) {
            out << castlingSymbol;
        }

        return out;
    }
};

void PositionFen::fenEnPassant(io::ostream& out) const {
    if (!OP.hasEnPassant()) {
        out << '-';
        return;
    }

    out << Square{OP.enPassantFile(), colorToMove.is(White) ? Rank6 : Rank3};
}

io::ostream& operator << (io::ostream& out, const PositionFen& pos) {
    auto whiteSide = pos.sideOf(White);
    auto& whitePieces = pos[whiteSide];
    auto& blackPieces = pos[~whiteSide];

    out << WriteFenBoard(whitePieces, blackPieces)
        << ' '
        << pos.colorToMove
        << ' '
        << WriteFenCastling(whitePieces, blackPieces, pos.chessVariant)
        << ' ';
    pos.fenEnPassant(out);
    return out;
}

void PositionFen::write(io::ostream& out, Move move) const {
    Move::write(out, move, colorToMove, chessVariant);
}

void PositionFen::write(io::ostream& out, const Move pv[]) const {
    Move::write(out, pv, colorToMove, chessVariant);
}

Move PositionFen::readMove(io::istream& in) const {
    Square moveFrom;
    Square moveTo;

    auto here = in.tellg();
    in >> std::ws >> moveFrom >> moveTo;

    if (!in) {
        io::fail_pos(in, here);
        return {};
    }

    if (colorToMove.is(Black)) {
        moveFrom.flip();
        moveTo.flip();
    }

    if (!MY.hasPieceOn(moveFrom)) {
        io::fail_pos(in, here);
        return {};
    }

    Pi pi = MY.pieceOn(moveFrom);

    //convert special moves (castling, promotion, ep) to the internal move format
    if (MY.isPawn(pi)) {
        if (moveFrom.on(Rank7)) {
            PromoType promo{Queen};
            in >> promo;
            in.clear(); //promotion piece is optional
            return Move::promotion(moveFrom, moveTo, promo);
        }
        if (moveFrom.on(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(moveTo))) {
            return Move::enPassant(moveFrom, Square{File(moveTo), Rank5});
        }
        //else normal pawn move
    }
    else if (pi.is(TheKing)) {
        if (MY.hasPieceOn(moveTo)) { //Chess960 castling encoding
            if (!MY.isCastling(moveTo)) {
                io::fail_pos(in, here);
                return {};
            }
            return Move::castling(moveTo, moveFrom);
        }
        if (moveFrom.is(E1) && moveTo.is(G1)) {
            if (!MY.hasPieceOn(H1) || !MY.isCastling(H1)) {
                io::fail_pos(in, here);
                return {};
            }
            return Move::castling(H1, E1);
        }
        if (moveFrom.is(E1) && moveTo.is(C1)) {
            if (!MY.hasPieceOn(A1) || !MY.isCastling(A1)) {
                io::fail_pos(in, here);
                return {};
            }
            return Move::castling(A1, E1);
        }
        //else normal king move
    }

    return Move(moveFrom, moveTo);
}

void PositionFen::limitMoves(io::istream& in) {
    MatrixPiBb searchMoves;
    searchMoves.clear();
    index_t limit = 0;

    while (in) {
        auto here = in.tellg();

        Move move = readMove(in);
        if (in) {
            Square from = move.from();
            Square to = move.to();
            if (isLegalMove(from, to)) {
                Pi pi = MY.pieceOn(from);
                if (!searchMoves.has(pi, to)) {
                    searchMoves.set(pi, to);
                    ++limit;
                    continue;
                }
            }
        }

        io::fail_pos(in, here);
    }

    if (limit > 0) {
        setMoves(searchMoves);
        in.clear();
        return;
    }

    if (in.eof()) {
        io::fail_rewind(in);
    }
}

void PositionFen::playMoves(io::istream& in) {
    while (in) {
        auto here = in.tellg();

        Move move = readMove(in);
        if (in) {
            Square from{ move.from() };
            Square to{ move.to() } ;

            if (isLegalMove(from, to)) {
                playMove(from, to);
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, here);
    }
}

void PositionFen::setBoard(io::istream& in) {
    FenBoard board;

    in >> board >> std::ws;
    if (in.eof()) {
        //missing board data
        io::fail_rewind(in);
        return;
    }

    if (!in) {
        //specific board character is invalid
        io::fail(in);
        return;
    }

    auto here = in.tellg();
    in >> colorToMove;

    if (!in) {
        //invalid side to move
        io::fail_rewind(in);
        return;
    }

    Position pos;

    if (!board.dropPieces(pos, colorToMove)) {
        //missing one or both kings
        io::fail_rewind(in);
        return;
    }

    if (!pos.afterDrop()) {
        //the side to move king left in check
        io::fail_pos(in, here);
        return;
    }

    static_cast<Position&>(*this) = pos;
}

bool PositionFen::setCastling(Side My, File file) {
    return MY.setValidCastling(file);
}

bool PositionFen::setCastling(Side My, CastlingSide castlingSide) {
    return MY.setValidCastling(castlingSide);
}

io::istream& PositionFen::setCastling(io::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') {
        in.ignore();
        return in;
    }

    for (io::char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color = std::isupper(c) ? White : Black;
            Side side = sideOf(color);

            c = static_cast<io::char_type>(std::tolower(c));

            CastlingSide castlingSide{CastlingSide::Begin};
            if (castlingSide.from_char(c)) {
                if (setCastling(side, castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if (file.from_char(c)) {
                    if (setCastling(side, file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}

bool PositionFen::setEnPassant(File file) {
    if (MY.hasEnPassant() || OP.hasEnPassant()) { return false; }
    if (MY.occupied().has(Square{file, Rank7})) { return false; }
    if (MY.occupied().has(Square{file, Rank6})) { return false; }

    Square victimSquare(file, Rank4);
    if (!OP.hasPieceOn(victimSquare)) { return false; }

    Pi victim = OP.pieceOn(victimSquare);
    if (!OP.isPawn(victim)) { return false; }

    //check against illegal en passant set field like "8/5bk1/8/2Pp4/8/1K6/8/8 w - d6"
    if (OP.isPinned(OP.occupied() - victimSquare)) {
        return false;
    }

    setLegalEnPassant<Op>(victim, victimSquare);
    return true;
}

io::istream& PositionFen::setEnPassant(io::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') {
        in.ignore();
        return in;
    }

    Square ep;

    auto here = in.tellg();
    in >> ep;

    if (in) {
        if (!ep.on(colorToMove.is(White) ? Rank6 : Rank3) || !setEnPassant( File(ep) )) {
            io::fail_pos(in, here);
        }
    }
    return in;
}

void PositionFen::readFen(io::istream& in) {
    setBoard(in);
    setCastling(in);
    setEnPassant(in);
    setZobrist();
    generateMoves();

    if (in) {
        unsigned _fifty;
        unsigned _moves;
        in >> _fifty >> _moves;
        in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }
}

void PositionFen::setStartpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    readFen(startpos);
}

#undef MY
#undef OP
