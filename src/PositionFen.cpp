#include "PositionFen.hpp"

#include <cctype>
#include <set>
#include "Position.hpp"

namespace {
typedef std::istream::char_type char_type;

class ColorTypeSquares {
    struct SquareOrder {
        bool operator () (Square, Square) const;
    };
    typedef std::set<Square, SquareOrder> Squares;

    Color::array< PieceType::array<Squares> > colorTypeSquares;

public:
    bool drop(Color, PieceType, Square);
    void readBoard(std::istream&);
    bool setupBoard(Position&, Color);
};

bool ColorTypeSquares::SquareOrder::operator () (Square sq1, Square sq2) const {
    if (Rank(sq1) != Rank(sq2)) {
        return Rank(sq1) < Rank(sq2); //Rank8 < Rank1
    }
    else {
        constexpr std::array<index_t, 8> order{6, 4, 2, 0, 1, 3, 5, 7};
        return order[File(sq1)] < order[File(sq2)]; //FileD < FileE < FileC < FileF < FileB < FileG < FileA < FileH
    }
}

bool ColorTypeSquares::drop(Color color, PieceType ty, Square sq) {
    if (ty == King) {
        //the king should be only one per each color
        if (!colorTypeSquares[color][King].empty()) {
            return false;
        }
    }
    else if (ty == Pawn) {
        //pawns should not occupy first and last ranks
        if (sq.is<Rank1>() || sq.is<Rank8>()) {
            return false;
        }
    }

    colorTypeSquares[color][ty].insert((color == White)? sq:~sq);
    return true;
}

void ColorTypeSquares::readBoard(std::istream& in) {
    in >> std::ws;

    Color::array<index_t> colorCount;
    colorCount.fill(0);

    File file{FileA}; Rank rank{Rank8};
    for (char_type c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c)? White:Black;
            c = static_cast<char_type>(std::tolower(c));

            PieceType ty{PieceType::Begin};
            if (ty.from_char(c) && colorCount[color] < Pi::Size) {
                if (drop(color, ty, Square(file, rank))) {
                    ++colorCount[color];
                    ++file;
                    continue;
                }
            }
            ::fail_char(in);
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
            ::fail_char(in);
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
        ::fail_char(in);
    }
}

bool ColorTypeSquares::setupBoard(Position& pos, Color colorToMove) {
    pos = {};

    //kings should be placed before any opponent's non king piece
    FOR_INDEX(Color, color) {
        if (colorTypeSquares[color][King].empty()) {
            return false;
        }

        auto king = colorTypeSquares[color][King].begin();
        if ( pos.drop( (color == colorToMove)? My:Op, King, *king) ) {
            colorTypeSquares[color][King].erase(king);
        }
        else {
            return false;
        }

        assert (colorTypeSquares[color][King].empty());
    }

    FOR_INDEX(Color, color) {
        FOR_INDEX(PieceType, ty) {
            while ( !colorTypeSquares[color][ty].empty() ) {
                auto piece = colorTypeSquares[color][ty].begin();

                if ( pos.drop( (color == colorToMove)? My:Op, ty, *piece) ) {
                    colorTypeSquares[color][ty].erase(piece);
                }
                else {
                    return false;
                }
            }

        }
    }

    return pos.setup();
}

std::istream& readCastling(std::istream& in, Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    for (char_type c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color(std::isupper(c)? White:Black);
            c = static_cast<char_type>(std::tolower(c));

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
        ::fail_char(in);
    }
    return in;
}

std::istream& readEnPassant(std::istream& in, Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    Square ep{Square::Begin};
    if (in >> ep) {
        if (colorToMove == White) { ep.flip(); }
        assert (ep.is<Rank3>());
        pos.setEnPassant(ep);
    }
    return in;
}

void writeBoard(std::ostream& out, const PositionSide& white, const PositionSide& black) {
    FOR_INDEX(Rank, rank) {
        index_t blank_squares = 0;

        FOR_INDEX(File, file) {
            Square sq(file,rank);

            if (white[sq]) {
                if (blank_squares != 0) { out << blank_squares; blank_squares = 0; }
                out << static_cast<char>(std::toupper( white.typeOf(sq).to_char() ));
            }
            else if (black[~sq]) {
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
}

class CastlingSet {
    std::set<char_type> castlingSet;

    void insert(const PositionSide& side, Color color, ChessVariant chessVariant);

public:
    CastlingSet (const PositionSide& white, const PositionSide& black, ChessVariant chessVariant) {
        insert(white, White, chessVariant);
        insert(black, Black, chessVariant);
    }

    void write(std::ostream&) const;
};

void CastlingSet::insert(const PositionSide& side, Color color, ChessVariant chessVariant) {
    for (Pi pi : side.castlingRooks()) {
        char_type castling_symbol =
            (chessVariant == Chess960)
            ? File{side.squareOf(pi)}.to_char()
            : side.castlingSideOf(pi).to_char()
        ;

        if (color == White) {
            castling_symbol = static_cast<char_type>(std::toupper(castling_symbol));
        }

        castlingSet.insert(castling_symbol);
    }
}

void CastlingSet::write(std::ostream& out) const {
    if (castlingSet.empty()) {
        out << '-';
    }
    else {
        for (auto castling_symbol : castlingSet) { out << castling_symbol; }
    }
}

void writeEnPassant(std::ostream& out, const PositionSide& side, Color colorToMove) {
    if (side.hasEnPassant()) {
        Square ep = side.enPassantSquare().rankDown();
        if (colorToMove == White) { ep.flip(); }
        out << ep;
    }
    else {
        out << '-';
    }
}

} //end of anonymous namespace

namespace PositionFen {

void read(std::istream& in, Position& pos, Color& colorToMove) {
    ColorTypeSquares colorTypeSquares;

    colorTypeSquares.readBoard(in);
    in >> std::ws >> colorToMove;

    if (in && !colorTypeSquares.setupBoard(pos, colorToMove)) {
        ::fail_char(in);
    }

    readCastling(in, pos, colorToMove);
    readEnPassant(in, pos, colorToMove);

    if (in) {
        unsigned fifty, moves;
        in >> fifty >> moves;
        in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
    }
}

void write(std::ostream& out, const Position& pos, Color colorToMove, ChessVariant chessVariant) {
    const PositionSide& white = pos.side[(colorToMove == White)? My:Op];
    const PositionSide& black = pos.side[(colorToMove == Black)? My:Op];

    writeBoard(out, white, black);

    out << ' ';
    out << colorToMove;

    out << ' ';
    CastlingSet{white, black, chessVariant}.write(out);

    out << ' ';
    writeEnPassant(out, pos.side[Op], colorToMove);
}

} //end of PositionFen namespace
