#include "FenBoard.hpp"
#include "Position.hpp"

std::istream& operator >> (std::istream& in, FenBoard& board) {
    in >> std::ws;

    Color::array<index_t> colorCount;
    colorCount.fill(0);

    File file{FileA}; Rank rank{Rank8};
    for (io::char_type c; in.get(c); ) {
        if (std::isalpha(c) && rank.isOk() && file.isOk()) {
            Color color = std::isupper(c)? White : Black;
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
    else if (ty.is(Pawn)) {
        //pawns should not occupy first and last ranks
        if (sq.is(Rank1) || sq.is(Rank8)) {
            return false;
        }
    }

    pieces[color][ty].insert(color.is(White)? sq : ~sq);
    return true;
}

bool FenBoard::setPosition(Position* p, Color colorToMove) {
    auto& pos = *p;
    pos = Position(0);

    //TRICK: kings should be placed before any opponent's non king pieces
    FOR_INDEX(Color, color) {
        if (pieces[color][King].empty()) {
            return false;
        }

        Side si(color.is(colorToMove)? My : Op);

        auto king = pieces[color][King].begin();

        if (pos.drop(si, King, *king)) {
            pieces[color][King].erase(king);
        }
        else {
            return false;
        }

        assert (pieces[color][King].empty());
    }

    FOR_INDEX(Color, color) {
        Side si(color.is(colorToMove)? My : Op);

        FOR_INDEX(PieceType, ty) {
            while ( !pieces[color][ty].empty() ) {
                auto piece = pieces[color][ty].begin();

                if (pos.drop(si, ty, *piece)) {
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

std::ostream& FenBoard::write(std::ostream& out, const PositionSide& white, const PositionSide& black) {
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
