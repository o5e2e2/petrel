#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (UciOutput& o) : SearchControl(o) {}

void UciControl::go(io::istream& command, const UciPosition& position) {
    if (!isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchLimit.readUci(command, position);
    SearchControl::go();
}

void UciControl::uciok() {
    static_cast<UciOutput&>(info).uciok( this->tt().getInfo() );
}

void UciControl::readyok() {
    static_cast<UciOutput&>(info).isready( this->isReady() );
}
