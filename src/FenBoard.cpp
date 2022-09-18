#include "FenBoard.hpp"
#include "Position.hpp"

io::istream& operator >> (io::istream& in, FenBoard& board) {
    in >> std::ws;

    File file{FileA}; Rank rank{Rank8};
    for (io::char_type c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c) ? White : Black;
            c = static_cast<io::char_type>(std::tolower(c));

            PieceType ty{PieceType::Begin};
            if ( ty.from_char(c) && board.drop(color, ty, Square{file, rank}) ) {
                ++file;
                continue;
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

bool FenBoard::drop(Color color, PieceType ty, Square sq) {
    //our position representaion cannot hold more then 16 total pieces per color
    if (pieceCount[color] == Pi::Size) {
        return false;
    }

    //max one king per each color
    if (ty.is(King) && !pieces[color][King].empty()) {
        return false;
    }

    //illegal pawn location
    if (ty.is(Pawn) && (sq.on(Rank1) || sq.on(Rank8))) {
        return false;
    }

    ++pieceCount[color];
    pieces[color][ty].insert(color.is(White) ? sq : ~sq);
    return true;
}

bool FenBoard::dropPieces(Position& position, Color colorToMove) {
    //each side should have one king
    FOR_INDEX(Color, color) {
        if (pieces[color][King].empty()) {
            return false;
        }
    }

    Position pos;

    FOR_INDEX(Color, color) {
        Side side = colorToMove.is(color) ? My : Op;

        FOR_INDEX(PieceType, ty) {
            while (!pieces[color][ty].empty()) {
                auto piece = pieces[color][ty].begin();

                if (!pos.dropValid(side, ty, *piece)) {
                    //should never happen
                    return false;
                }

                pieces[color][ty].erase(piece);
            }
        }
    }

    position = pos;
    return true;
}
