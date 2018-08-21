#include <set>

#include "PositionFen.hpp"
#include "FenBoard.hpp"
#include "CastlingRules.hpp"

#define MY side[My]
#define OP side[Op]

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

    void insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
        for (Pi pi : side.castlingRooks()) {
            io::char_type castlingSymbol;

            switch (chessVariant) {
                case Chess960:
                    castlingSymbol = File{side.squareOf(pi)}.to_char();
                    break;

                case Orthodox:
                default:
                    castlingSymbol = CastlingRules::castlingSide(side.kingSquare(), side.squareOf(pi)).to_char();
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

    out << Square(OP.enPassantFile(), colorToMove.is(White) ? Rank6 : Rank3);
}

io::ostream& operator << (io::ostream& out, const PositionFen& pos) {
    auto whiteSide = pos.colorToMove.is(White) ? My : Op;
    auto& whitePieces = pos.side[whiteSide];
    auto& blackPieces = pos.side[~whiteSide];

    out << WriteFenBoard(whitePieces, blackPieces)
        << ' '
        << pos.colorToMove
        << ' '
        << WriteFenCastling(whitePieces, blackPieces, pos.chessVariant)
        << ' ';
    pos.fenEnPassant(out);
    return out;
}

Move PositionFen::readMove(io::istream& in) const {
    auto before = in.tellg();

    Square moveFrom{Square::Begin};
    Square moveTo{Square::Begin};
    if (in >> std::ws >> moveFrom >> moveTo) {
        if (colorToMove.is(Black)) {
            moveFrom.flip();
            moveTo.flip();
        }

        if (MY.isPieceOn(moveFrom)) {
            Pi pi{MY.pieceOn(moveFrom)};

            //convert special moves (castling, promotion, ep) to the internal move format
            if (MY.isPawn(pi)) {
                if (moveFrom.is(Rank7)) {
                    PromoType promo{Queen};
                    in >> promo;
                    in.clear(); //promotion piece is optional
                    return Move::promotion(moveFrom, moveTo, promo);
                }
                if (moveFrom.is(Rank5) && OP.hasEnPassant() && OP.enPassantFile().is(File(moveTo))) {
                    return Move::enPassant(moveFrom, Square{File(moveTo), Rank5});
                }
            }
            else if (pi.is(TheKing)) {
                if (MY.isPieceOn(moveTo)) { //Chess960 castling encoding
                    return Move::castling(moveTo, moveFrom);
                }
                if (moveFrom.is(E1) && moveTo.is(G1) && MY.isPieceOn(H1)) {
                    return Move::castling(H1, E1);
                }
                if (moveFrom.is(E1) && moveTo.is(C1) && MY.isPieceOn(A1)) {
                    return Move::castling(A1, E1);
                }
            }

            return Move(moveFrom, moveTo);
        }
    }

    io::fail_pos(in, before);
    return Move{};
}

void PositionFen::limitMoves(io::istream& in) {
    MatrixPiBb searchMoves;
    searchMoves.clear();
    index_t limit = 0;

    while (in) {
        auto before = in.tellg();

        Move move = readMove(in);
        if (in) {
            Square from{ move.from() };
            Square to{ move.to() } ;
            Pi pi{ MY.pieceOn(from) };

            if (moves.is(pi, to) && !searchMoves.is(pi, to)) {
                searchMoves.set(pi, to);
                ++limit;
                continue;
            }

        }

        io::fail_pos(in, before);
    }

    if (limit > 0) {
        moves = searchMoves;
        in.clear();
        return;
    }

    if (in.eof()) {
        io::fail_rewind(in);
    }
}

void PositionFen::playMoves(io::istream& in) {
    while (in) {
        auto before = in.tellg();

        Move move = readMove(in);
        if (in) {
            Square from{ move.from() };
            Square to{ move.to() } ;
            Pi pi{ side[My].pieceOn(from) };

            if (isLegalMove(pi, to)) {
                Position::playMove(from, to);
                generateMoves();
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before);
    }
}

void PositionFen::setBoard(io::istream& in) {
    FenBoard board;

    in >> board >> std::ws >> colorToMove;

    if (in && !board.setPosition(*this, colorToMove)) {
        io::fail_char(in);
    }
}

io::istream& PositionFen::setCastling(io::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    for (io::char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color = std::isupper(c) ? White : Black;
            Side colorSide = color.is(colorToMove) ? My : Op;

            c = static_cast<io::char_type>(std::tolower(c));

            CastlingSide castlingSide{CastlingSide::Begin};
            if (castlingSide.from_char(c)) {
                if (Position::setCastling(colorSide, castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if (file.from_char(c)) {
                    if (Position::setCastling(colorSide, file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}

io::istream& PositionFen::setEnPassant(io::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    auto before = in.tellg();

    Square ep{Square::Begin};
    if (in >> ep) {
        if (!ep.is(colorToMove.is(White) ? Rank6 : Rank3) || !Position::setEnPassant(File{ep})) {
            io::fail_pos(in, before);
        }
    }
    return in;
}

void PositionFen::readFen(io::istream& in) {
    setBoard(in);
    setCastling(in);
    setEnPassant(in);
    zobrist = generateZobrist();
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

void PositionFen::readUci(io::istream& command) {
    if (io::next(command, "startpos")) { setStartpos(); }
    if (io::next(command, "fen")) { readFen(command); }

    io::next(command, "moves");
    playMoves(command);
}

#undef MY
#undef OP
