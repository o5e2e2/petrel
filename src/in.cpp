#include "in.hpp"

namespace in {

istream& fail(istream& in) {
    in.setstate(std::ios::failbit);
    return in;
}

istream& fail_char(istream& in) {
    in.unget();
    return fail(in);
}

istream& fail_pos(istream& in, std::streampos here) {
    in.clear();
    in.seekg(here);
    return fail(in);
}

istream& fail_rewind(istream& in) {
    return fail_pos(in, 0);
}

/// Read the input stream and try to match it with the given token parameter.
/// The comparision is case insensitive and all sequential space symbols are equal to single space.
/// @retval true: stream matches the given token, stream position is set forward past the matched token
/// @retval false: failed to match stream with the given token, stream state reset back before call
bool next(istream& in, czstring token) {
    if (token == nullptr) { token = ""; }

    auto state = in.rdstate();
    auto before = in.tellg();

    using std::isspace;
    do {
        while ( isspace(*token) ) { ++token; }
        in >> std::ws;

        while ( !isspace(*token) && std::tolower(*token) == std::tolower(in.peek()) ) {
            ++token; in.ignore();
        }
    } while ( isspace(*token) && isspace(in.peek()) );

    if ( *token == '\0' && (isspace(in.peek()) || in.eof()) ) {
        return true;
    }

    in.seekg(before);
    in.clear(state);
    return false;
}

bool nextNothing(istream& in) {
    in >> std::ws;
    return in.eof();
}

} // namespace in
