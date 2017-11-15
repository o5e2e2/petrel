#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (UciOutput& o) : SearchControl(o) {}

void UciControl::go(std::istream& command, const UciPosition& uciPosition) {
    searchLimit.readUci(command, uciPosition, &position);
    SearchControl::go(position, searchLimit);
}

void UciControl::readUciHash(std::istream& command) {
    HashMemory::size_t quantity = 0;
    if (!(command >> quantity)) {
        io::fail_rewind(command);
        return;
    }

    io::char_type u = 'm';
    command >> u;

    switch (std::tolower(u)) {
        case 'g': quantity *= 1024;
        case 'm': quantity *= 1024;
        case 'k': quantity *= 1024;
        case 'b': break;

        default: {
            io::fail_rewind(command);
            return;
        }
    }

    tt().resize(quantity);
}

void UciControl::uciok() {
    static_cast<UciOutput&>(info).uciok( this->tt() );
}

void UciControl::readyok() {
    static_cast<UciOutput&>(info).isready( this->isReady() );
}
