#include "UciHash.hpp"
#include "HashMemory.hpp"
#include "PerftTT.hpp"

namespace {
    enum { MiB = 1024 * 1024 };
    constexpr UciHash::_t toMiB(HashMemory::size_t bytes) { return small_cast<UciHash::_t>(bytes / MiB); }
}

UciHash::UciHash (HashMemory& hash) : hashMemory(hash) {}

void UciHash::UciHash::resize(_t mb) {
    hashMemory.resize(static_cast<HashMemory::size_t>(mb) * MiB);
    PerftTT::resetAge();
}

void UciHash::newGame() {
    hashMemory.clear();
    PerftTT::resetAge();
}

std::ostream& UciHash::option(std::ostream& out) const {
    auto current = hashMemory.getSize();
    auto max = hashMemory.getMax();

    out << "option name Hash type spin min 0 max " << toMiB(max) << " default " << toMiB(current) << '\n';

    return out;
}

std::ostream& UciHash::hashfull(std::ostream& out) const {
    auto hf = (static_cast<HashMemory::size_t>(PerftTT::getUsed())*1000) / hashMemory.getTotalRecords();

    if (hf > 0) {
        out << " hashfull " << hf;
    }

    return out;
}
