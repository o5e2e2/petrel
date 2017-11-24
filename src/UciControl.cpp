#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (UciOutput& o) : SearchControl(o) {}

void UciControl::readUciHash(std::istream& command) {
    HashMemory::size_t quantity = 0;
    command >> quantity;
    if (!command) {
        io::fail_rewind(command);
        return;
    }

    io::char_type unit = 'm';
    command >> unit;

    switch (std::tolower(unit)) {
        case 't': quantity *= 1024;
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
