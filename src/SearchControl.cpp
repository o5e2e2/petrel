#include "SearchControl.hpp"
#include "SearchInfo.hpp"
#include "SearchLimit.hpp"

SearchControl::SearchControl (SearchInfo& i) : info(i), rootWindow(*this) { clear(); }

void SearchControl::readUciHash(std::istream& command) {
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

    transpositionTable.resize(quantity);
}

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

bool SearchControl::checkQuota() {
    return info.checkQuota(searchThread);
}

void SearchControl::go(const PositionMoves& rootMoves, const SearchLimit& searchLimit) {
    info.clear();
    info.setNodesLimit( searchLimit.getNodes() );

    rootWindow.draft = searchLimit.getDepth();
    auto rootSearch = (rootWindow.draft > 0)? Perft::perftRoot : Perft::perftId;
    rootWindow.searchFn = searchLimit.getDivide()? Perft::divide : Perft::perft;

    searchThread.set(rootSearch, rootMoves, rootWindow);
    searchSequence = searchThread.commandRun();

    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
