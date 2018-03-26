#include "FenBoard.hpp"
#include "Position.hpp"

io::istream& operator >> (io::istream& in, FenBoard& board) {
    in >> std::ws;

    Color::array<index_t> colorCount;
    colorCount.fill(0);

    File file{FileA}; Rank rank{Rank8};
    for (io::char_type c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c) ? White : Black;
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

bool FenBoard::drop(Color color, PieceType ty, Square sq) {
    if (ty.is(King)) {
        //the king should be only one per each color
        if (!pieces[color][King].empty()) {
            return false;
        }
    }

    pieces[color][ty].insert(color.is(White) ? sq : ~sq);
    return true;
}

bool FenBoard::setPosition(Position& position, Color colorToMove) {
    //each side should have one king
    FOR_INDEX(Color, color) {
        if (pieces[color][King].empty()) {
            return false;
        }
    }

    Position pos;
    pos.clear();

    FOR_INDEX(Color, color) {
        Side colorSide = color.is(colorToMove) ? My : Op;

        FOR_INDEX(PieceType, ty) {
            while (!pieces[color][ty].empty()) {
                auto piece = pieces[color][ty].begin();

                if (!pos.drop(colorSide, ty, *piece)) {
                    return false;
                }

                pieces[color][ty].erase(piece);
            }
        }
    }

    if (!pos.setup()) {
        return false;
    }

    position = pos;
    return true;
}
