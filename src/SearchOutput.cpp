#include <iomanip>
#include <ostream>

#include "SearchOutput.hpp"
#include "Bb.hpp"
#include "VectorPiSquare.hpp"

std::ostream& operator << (std::ostream& out, Bb bb) {
    FOR_INDEX(Rank, rank) {
        FOR_INDEX(File, file) {
            out << (bb[Square(file, rank)]? file:'.');
        }
        out << '\n';
    }
    return out;
}

std::ostream& operator << (std::ostream& out, VectorPiSquare squares) {
    auto flags = out.flags();

    out << std::hex;
    FOR_INDEX(Pi, pi) {
        out << ' ' << pi;
    }
    out << '\n';

    FOR_INDEX(Pi, pi) {
        out << std::setw(2) << static_cast<unsigned>(small_cast<unsigned char>((squares._v[pi])));
    }
    out << '\n';

    out.flags(flags);
    return out;
}
