#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (const UciPosition& pos, io::ostream& out, io::ostream& err)
:
    SearchControl(this->uciOutput),
    uciOutput(pos, out, err)
{}

void UciControl::error(io::istream& command) const {
    uciOutput.error(command);
}

void UciControl::info_fen() const {
    uciOutput.info_fen();
}

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
