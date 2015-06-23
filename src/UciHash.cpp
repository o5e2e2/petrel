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

std::ostream& UciHash::option(std::ostream& out) const {
    auto current = hashMemory.getSize();
    auto max = hashMemory.getMax();

    out << "option name Hash type spin min 0 max " << toMiB(max) << " default " << toMiB(current) << '\n';

    return out;
}

std::ostream& UciHash::hashfull(std::ostream& out) const {
    auto used  = PerftTT::getUsed();
    auto total = hashMemory.getTotalRecords();

    if (used > 0) {
        auto hf = (static_cast<HashMemory::size_t>(used)*1000) / total;
        hf = std::min(hf, static_cast<decltype(hf)>(1000));
        out << " hashfull " << hf;
    }

    return out;
}
