#include "out.hpp"

namespace out {

ostream& space(ostream& out, unsigned n, char_type c) {
    if (n > 16) { out << '~'; }
    for (auto i = 0u; i < n; ++i) { out << c; }
    return out;
}

} // namespace out
