#include "PositionFen.hpp"

#include <cctype>
#include "Position.hpp"

bool PositionFen::SquareOrder::operator () (Square sq1, Square sq2) const {
    if (Rank(sq1) != Rank(sq2)) {
        return Rank(sq1) < Rank(sq2); //Rank8 < Rank1
    }
    else {
        constexpr std::array<index_t, 8> pr{6, 4, 2, 0, 1, 3, 5, 7};
        return pr[File(sq1)] < pr[File(sq2)]; //FileD < FileE < FileC < FileF < FileB < FileG < FileA < FileH
    }
}

Color PositionFen::readFen(std::istream& in) {
    ColorTypeSquares colorTypeSquares;
    Color colorToMove{White};

    in >> std::ws;
    readBoard(in, colorTypeSquares) >> std::ws >> colorToMove >> std::ws;

    if (in) {
        ::fail_if_not(setupBoard(colorTypeSquares, colorToMove), in);
        readCastling(in, colorToMove) >> std::ws;
        readEnPassant(in, colorToMove) >> std::ws;
        if (in) {
            unsigned fifty, moves;
            in >> fifty >> moves;
            in.clear(); //ignore missing optional 'fifty' and 'moves' fen fields
        }
    }

    return colorToMove;
}

std::istream& PositionFen::readBoard(std::istream& in, ColorTypeSquares& colorTypeSquares) {
    Color::array<index_t> colorCount;
    colorCount.fill(0);

    File file{FileA}; Rank rank{Rank8};
    for (char c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c)? White:Black;
            c = static_cast<char>(std::tolower(c));

            PieceType ty{PieceType::Begin};
            if (ty.from_char(c) && colorCount[color] < Pi::Size) {
                if (drop(colorTypeSquares, color, ty, Square{file, rank})) {
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
    return in;
}

bool PositionFen::drop(ColorTypeSquares& colorTypeSquares, Color color, PieceType ty, Square sq) {
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

    //TODO: sort pieces color
    //color independent piece coordinates
    colorTypeSquares[color][ty].insert((color == White)? sq:~sq);
    return true;
}

bool PositionFen::setupBoard(ColorTypeSquares& colorTypeSquares, Color colorToMove) {
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

std::istream& PositionFen::readCastling(std::istream& in, Color colorToMove) {
    if (in.peek() == '-') { in.ignore(); return in; }

    for (char c; in.get(c) && !std::isblank(c); ) {
        if (std::isalpha(c)) {
            Color color(std::isupper(c)? White:Black);
            c = static_cast<char>(std::tolower(c));

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
        break;
    }
    return in;
}

std::istream& PositionFen::readEnPassant(std::istream& in, Color colorToMove) {
    if (in.peek() == '-') { in.ignore(); return in; }

    Square ep{Square::Begin};
    if (in >> ep) {
        if (colorToMove == White) { ep.flip(); }
        assert (ep.is<Rank3>());
        pos.setEnPassant(ep);
    }
    return in;
}
