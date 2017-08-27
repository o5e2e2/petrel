#include <set>

#include "UciPosition.hpp"
#include "FenBoard.hpp"
#include "CastlingRules.hpp"

#define MY side[My]
#define OP side[Op]

class WriteFenBoard {
    const PositionSide& whiteSide;
    const PositionSide& blackSide;

public:
    WriteFenBoard (const UciPosition& pos) :
        whiteSide(pos.getSide(White)),
        blackSide(pos.getSide(Black))
        {}

    friend std::ostream& operator << (std::ostream& out, const WriteFenBoard& fen) {
        FOR_INDEX(Rank, rank) {
            index_t blank_squares = 0;

            FOR_INDEX(File, file) {
                Square sq(file,rank);

                if (fen.whiteSide.isOccupied(sq)) {
                    if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                    out << static_cast<io::char_type>(std::toupper( fen.whiteSide.typeOf(sq).to_char() ));
                    continue;
                }

                if (fen.blackSide.isOccupied(~sq)) {
                    if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                    out << fen.blackSide.typeOf(~sq).to_char();
                    continue;
                }

                ++blank_squares;
            }

            if (blank_squares != 0) { out << blank_squares; }
            if (rank != Rank1) { out << '/'; }
        }

        return out;
    }
};

class WriteFenCastling {
    std::set<io::char_type> castlingSet;

    void insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
        for (Pi pi : side.castlingRooks()) {
            io::char_type castling_symbol;

            switch (chessVariant) {
                case Chess960:
                    castling_symbol = File{side.squareOf(pi)}.to_char();
                    break;

                case Orthodox:
                default:
                    castling_symbol = CastlingRules::castlingSide(side.kingSquare(), side.squareOf(pi)).to_char();
                    break;
            }

            if (color.is(White)) {
                castling_symbol = static_cast<io::char_type>(std::toupper(castling_symbol));
            }

            castlingSet.insert(castling_symbol);
        }
    }

public:
    WriteFenCastling (const UciPosition& pos) {
        insert(pos.getSide(White), White, pos.getChessVariant());
        insert(pos.getSide(Black), Black, pos.getChessVariant());
    }

    friend std::ostream& operator << (std::ostream& out, const WriteFenCastling& fen) {
        if (fen.castlingSet.empty()) {
            return out << '-';
        }

        for (auto castling_symbol : fen.castlingSet) {
            out << castling_symbol;
        }

        return out;
    }
};

void UciPosition::fenEnPassant(std::ostream& out) const {
    if (!OP.hasEnPassant()) {
        out << '-';
        return;
    }

    out << Square(OP.enPassantFile(), colorToMove.is(White)? Rank6 : Rank3);
}

std::ostream& operator << (std::ostream& out, const UciPosition& pos) {
    out << WriteFenBoard(pos)
        << ' '
        << pos.colorToMove
        << ' '
        << WriteFenCastling(pos)
        << ' ';
    pos.fenEnPassant(out);
    return out;
}

Move UciPosition::readMove(std::istream& in) const {
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

void UciPosition::limitMoves(std::istream& in) {
    MatrixPiBb searchMoves;
    index_t limit = 0;

    while (in) {
        auto before = in.tellg();

        Move m = readMove(in);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
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

void UciPosition::playMoves(std::istream& in) {
    while (in) {
        auto before = in.tellg();

        Move m = readMove(in);
        if (in) {
            Square from{ m.from() };
            Square to{ m.to() } ;
            Pi pi{ side[My].pieceOn(from) };

            if (is(pi, to)) {
                Position::playMove(from, to);
                generateMoves<My>();
                colorToMove.flip();
                continue;
            }
        }

        io::fail_pos(in, before);
    }
}

void UciPosition::setBoard(std::istream& in) {
    FenBoard board;

    in >> board >> std::ws >> colorToMove;

    if (in && !board.setPosition(*this, colorToMove)) {
        io::fail_char(in);
    }
}

std::istream& UciPosition::setCastling(std::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    for (io::char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color(std::isupper(c)? White : Black);
            Side si(color.is(colorToMove)? My : Op);

            c = static_cast<io::char_type>(std::tolower(c));

            CastlingSide castlingSide{CastlingSide::Begin};
            if (castlingSide.from_char(c)) {
                if (Position::setCastling(si, castlingSide)) {
                    continue;
                }
            }
            else {
                File file{File::Begin};
                if (file.from_char(c)) {
                    if (Position::setCastling(si, file)) {
                        continue;
                    }
                }
            }
        }
        io::fail_char(in);
    }
    return in;
}

std::istream& UciPosition::setEnPassant(std::istream& in) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    auto before = in.tellg();

    Square ep{Square::Begin};
    if (in >> ep) {
        if (!ep.is(colorToMove.is(White)? Rank6 : Rank3) || !Position::setEnPassant(File{ep})) {
            io::fail_pos(in, before);
        }
    }
    return in;
}

std::istream& operator >> (std::istream& in, UciPosition& pos) {
    pos.setBoard(in);
    pos.setCastling(in);
    pos.setEnPassant(in);
    pos.zobrist = pos.generateZobrist();
    pos.generateMoves<My>();

    if (in) {
        unsigned _fifty;
        unsigned _moves;
        in >> _fifty >> _moves;
        in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }

    return in;
}

#undef MY
#undef OP
