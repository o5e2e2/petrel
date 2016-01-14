#include "FenEnPassant.hpp"
#include "Position.hpp"
#include "PositionSide.hpp"

std::istream& FenEnPassant::read(std::istream& in, Position& pos, Color colorToMove) {
    in >> std::ws;
    if (in.peek() == '-') { in.ignore(); return in; }

    Square ep{Square::Begin};
    if (in >> ep) {
        (void)colorToMove; //silence unused parameter warning
        assert (colorToMove.is(White)? ep.is(Rank6) : ep.is(Rank3));
        pos.setEnPassant(File{ep});
    }
    return in;
}

std::ostream& FenEnPassant::write(std::ostream& out, const PositionSide& side, Color colorToMove) {
    if (side.hasEnPassant()) {
        File epFile = side.enPassantFile();
        out << (colorToMove.is(White)? Square(epFile, Rank6) : Square(epFile, Rank3));
    }
    else {
        out << '-';
    }
    return out;
}
