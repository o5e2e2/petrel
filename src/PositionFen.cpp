#include <set>

#include "PositionFen.hpp"
#include "Position.hpp"
#include "CastlingRules.hpp"

namespace {

struct SquareOrder {
    bool operator () (Square sq1, Square sq2) const {
        if (Rank(sq1) != Rank(sq2)) {
            return Rank(sq1) < Rank(sq2); //Rank8 < Rank1
        }
        else {
            constexpr std::array<index_t, 8> order{6, 4, 2, 0, 1, 3, 5, 7};
            return order[File(sq1)] < order[File(sq2)]; //FileD < FileE < FileC < FileF < FileB < FileG < FileA < FileH
        }
    }
};

class Board {
    typedef std::set<Square, SquareOrder> Squares;
    Color::array< PieceType::array<Squares> > pieces;

    bool drop(Color color, PieceType ty, Square sq) {
        if (ty == King) {
            //the king should be only one per each color
            if (!pieces[color][King].empty()) {
                return false;
            }
        }
        else if (ty == Pawn) {
            //pawns should not occupy first and last ranks
            if (sq.is(Rank1) || sq.is(Rank8)) {
                return false;
            }
        }

        pieces[color][ty].insert((color == White)? sq:~sq);
        return true;
    }

    friend std::istream& operator >> (std::istream&, Board&);
    static bool setPosition(Position&, Board&&, Color);

public:
    static std::istream& read(std::istream&, Position&, Color&);
    static std::ostream& write(std::ostream&, const PositionSide& white, const PositionSide& black);
};

std::istream& operator >> (std::istream& in, Board& board) {
    in >> std::ws;

    Color::array<index_t> colorCount;
    colorCount.fill(0);

    File file{FileA}; Rank rank{Rank8};
    for (io::char_type c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c)? White:Black;
            c = static_cast<io::char_type>(std::tolower(c));

            PieceType ty{PieceType::Begin};
            if (ty.from_char(c) && colorCount[color] < Pi::Size) {
                if (board.drop(color, ty, Square(file, rank))) {
                    ++colorCount[color];
                    ++file;
                    continue;
                }
            }
            io::fail_char(in);
        }
        else if ('1' <= c && c <= '8' && rank.isOk() && file.isOk()) {
            int b = c - '0'; //convert digit symbol to int

            //skip blank squares
            int f = file + b;
            if (f <= static_cast<int>(File::Size)) {
                //avoid out of range initialization check
                file = static_cast<File::_t>(f-1);
                ++file;
                continue;
            }
            io::fail_char(in);
        }
        else if (c == '/' && rank.isOk()) {
            ++rank;
            file = FileA;
            continue;
        }
        else if (std::isblank(c)) {
            break; //end of board description field
        }

        //otherwise it is an input error
        io::fail_char(in);
    }
    return in;
}

bool Board::setPosition(Position& pos, Board&& board, Color colorToMove) {
    auto&& pieces = board.pieces;
    pos = {};

    //kings should be placed before any opponent's non king pieces
    FOR_INDEX(Color, color) {
        if (pieces[color][King].empty()) {
            return false;
        }

        auto king = pieces[color][King].begin();
        if ( pos.drop( (color == colorToMove)? My:Op, King, *king) ) {
            pieces[color][King].erase(king);
        }
        else {
            return false;
        }

        assert (pieces[color][King].empty());
    }

    FOR_INDEX(Color, color) {
        FOR_INDEX(PieceType, ty) {
            while ( !pieces[color][ty].empty() ) {
                auto piece = pieces[color][ty].begin();

                if ( pos.drop( (color == colorToMove)? My:Op, ty, *piece) ) {
                    pieces[color][ty].erase(piece);
                }
                else {
                    return false;
                }
            }

        }
    }

    return pos.setup();
}

std::istream& Board::read(std::istream& in, Position& pos, Color& colorToMove) {
    Board board;

    in >> board >> std::ws >> colorToMove;

    if (in && !Board::setPosition(pos, std::move(board), colorToMove)) {
        io::fail_char(in);
    }
    return in;
}

std::ostream& Board::write(std::ostream& out, const PositionSide& white, const PositionSide& black) {
    FOR_INDEX(Rank, rank) {
        index_t blank_squares = 0;

        FOR_INDEX(File, file) {
            Square sq(file,rank);

            if (white.isOccupied(sq)) {
                if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                out << static_cast<io::char_type>(std::toupper( white.typeOf(sq).to_char() ));
            }
            else if (black.isOccupied(~sq)) {
                if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                out << black.typeOf(~sq).to_char();
            }
            else {
                ++blank_squares;
            }

        }

        if (blank_squares != 0) { out << blank_squares; }
        if (rank != Rank1) { out << '/'; }
    }
    return out;
}

class CastlingRights {
    std::set<io::char_type> castlingSet;

    void insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
        for (Pi pi : side.castlingRooks()) {
            io::char_type castling_symbol;

            if (chessVariant == Chess960) {
                castling_symbol = File{side.squareOf(pi)}.to_char();
            }
            else {
                castling_symbol = CastlingRules::castlingSide(side.kingSquare(), side.squareOf(pi)).to_char();
            }

            if (color == White) {
                castling_symbol = static_cast<io::char_type>(std::toupper(castling_symbol));
            }

            castlingSet.insert(castling_symbol);
        }
    }

public:
    CastlingRights (const PositionSide& white, const PositionSide& black, ChessVariant chessVariant) {
        insert(white, White, chessVariant);
        insert(black, Black, chessVariant);
    }

    static std::istream& read(std::istream& in, Position& pos, Color colorToMove) {
        in >> std::ws;
        if (in.peek() == '-') { in.ignore(); return in; }

        for (io::char_type c; in.get(c) && !std::isblank(c); ) {
            if (std::isalpha(c)) {
                Color color(std::isupper(c)? White:Black);
                c = static_cast<io::char_type>(std::tolower(c));

                CastlingSide side{CastlingSide::Begin};
                if ( side.from_char(c) ) {
                    if (pos.setCastling((color == colorToMove)? My:Op, side)) {
                        continue;
                    }
                }
                else {
                    File file{File::Begin};
                    if ( file.from_char(c) ) {
                        if (pos.setCastling((color == colorToMove)? My:Op, file)) {
                            continue;
                        }
                    }
                }
            }
            io::fail_char(in);
        }
        return in;
    }

    friend std::ostream& operator << (std::ostream& out, const CastlingRights& castlingRights) {
        if (castlingRights.castlingSet.empty()) {
            out << '-';
        }
        else {
            for (auto castling_symbol : castlingRights.castlingSet) { out << castling_symbol; }
        }
        return out;
    }

};

namespace EnPassantSquare {
    std::istream& read(std::istream& in, Position& pos, Color colorToMove) {
        in >> std::ws;
        if (in.peek() == '-') { in.ignore(); return in; }

        Square ep{Square::Begin};
        if (in >> ep) {
            (void)colorToMove; //silence unused parameter warning
            assert (colorToMove == White? ep.is(Rank6) : ep.is(Rank3));
            pos.setEnPassant(File{ep});
        }
        return in;
    }

    std::ostream& write(std::ostream& out, const PositionSide& side, Color colorToMove) {
        if (side.hasEnPassant()) {
            File epFile = side.enPassantFile();
            out << (colorToMove == White? Square(epFile, Rank6) : Square(epFile, Rank3));
        }
        else {
            out << '-';
        }
        return out;
    }
}

} //end of anonymous namespace

namespace PositionFen {
    void read(std::istream& in, Position& pos, Color& colorToMove) {
        Board::read(in, pos, colorToMove);
        CastlingRights::read(in, pos, colorToMove);
        EnPassantSquare::read(in, pos, colorToMove);

        if (in) {
            unsigned fifty, moves;
            in >> fifty >> moves;
            in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
        }
    }

    void setStartpos(Position& pos, Color& colorToMove) {
        std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
        read(startpos, pos, colorToMove);
    }

    void write(std::ostream& out, const Position& pos, Color colorToMove, ChessVariant chessVariant) {
        const PositionSide& white = pos.side[(colorToMove == White)? My:Op];
        const PositionSide& black = pos.side[(colorToMove == Black)? My:Op];

        Board::write(out, white, black);
        out << ' ' << colorToMove;
        out << ' ' << CastlingRights(white, black, chessVariant);
        out << ' ';
        EnPassantSquare::write(out, pos.side[Op], colorToMove);
    }

} //end of PositionFen namespace
