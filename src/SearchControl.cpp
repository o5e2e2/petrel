#include "SearchControl.hpp"
#include "SearchLimit.hpp"
#include "PositionMoves.hpp"
#include "Timer.hpp"

SearchControl::SearchControl (SearchOutput& out) : info(out), rootWindow(*this) { clear(); }

void SearchControl::clear() {
    info.clear();
    transpositionTable.clear();
}

void SearchControl::uciSetHash(std::istream& command) {

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

void SearchControl::nextIteration() {
    transpositionTable.nextAge();
}

void SearchControl::go(const PositionMoves& rootMoves, const SearchLimit& searchLimit) {
    info.clear();
    info.nodesLimit = searchLimit.getNodes();

    rootWindow.draft = searchLimit.getDepth();
    auto rootSearch = (rootWindow.draft > 0)? Perft::perftRoot : Perft::perftId;
    rootWindow.searchFn = searchLimit.getDivide()? Perft::divide : Perft::perft;

    searchThread.set(rootSearch, rootMoves, rootWindow);
    searchSequence = searchThread.commandRun();

    Timer::run(timerPool, searchLimit.getThinkingTime(), searchThread, searchSequence);
}
