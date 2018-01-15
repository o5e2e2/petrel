#include "UciControl.hpp"
#include "UciSearchInfo.hpp"

UciControl::UciControl (const UciPosition& pos, io::ostream& out, io::ostream& err)
:
    uciSearchInfo(pos, out, err),
    searchControl(uciSearchInfo)
{}

void UciControl::go(io::istream& command, const UciPosition& position) {
    if (!isReady()) {
        io::fail_rewind(command);
        return;
    }

    searchControl.searchLimit.readUci(command, position);
    searchControl.go();
}
