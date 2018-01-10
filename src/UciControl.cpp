#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (const UciPosition& pos, io::ostream& out, io::ostream& err)
:
    uciOutput(pos, out, err),
    searchControl(this->uciOutput)
{}

void UciControl::go(io::istream& command, const UciPosition& position) {
    if (!isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchControl.searchLimit.readUci(command, position);
    searchControl.go();
}
