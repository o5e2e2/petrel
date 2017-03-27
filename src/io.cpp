#include "io.hpp"

namespace io {

std::istream& fail_here(std::istream& in) {
    in.setstate(std::ios::failbit);
    return in;
}

std::istream& fail_char(std::istream& in) {
    in.unget();
    return io::fail_here(in);
}

std::istream& fail_pos(std::istream& in, std::streampos before) {
    in.clear();
    in.seekg(before);
    return io::fail_here(in);
}

std::istream& fail_rewind(std::istream& in) {
    return io::fail_pos(in, 0);
}

std::istream& skip_token(std::istream& in) {
    while (in && !std::isspace(in.peek())) {
        in.ignore();
    }
    return in;
}

/**
    * Tries to read the stream token and match it with the given token parameter
    * The comparision is case insensitive and all sequential space symbols treated equal to the single space
    * Return:
    * true  -- next stream token is matches to the given token, shift stream cursor past the matched token
    * false -- failed to read stram or failed to match next stream token with the given token, stream state is not changed
    **/
bool next(std::istream& in, io::literal token) {
    if (token == nullptr) { token = ""; }

    auto pos_before = in.tellg();
    auto state_before = in.rdstate();

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

    in.clear(state_before);
    in.seekg(pos_before);
    return false;
}

std::ostream& app_version(std::ostream& out) {
    out << "Petrel ";

#ifdef GIT_HASH
    out << GIT_HASH << ' ';
#endif

#ifdef GIT_DATE
    out << GIT_DATE;
#else
    typedef io::char_type _t;

    _t year[] {__DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10], '\0'};

    _t month[] {
        (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '1' :
        (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
        (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '1' : '0',

        (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')? '1' :
        (__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b')? '2' :
        (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')? '3' :
        (__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r')? '4' :
        (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')? '5' :
        (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')? '6' :
        (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')? '7' :
        (__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g')? '8' :
        (__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p')? '9' :
        (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '0' :
        (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
        (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '2' : '0',

        '\0'
    };

    _t day[] {((__DATE__[4] == ' ')? '0' : __DATE__[4]), __DATE__[5], '\0'};

    out << "build " << year << '-' << month << '-' << day;

#endif

#ifndef NDEBUG
    out << " DEBUG";
#endif

    return out;
}

std::ostream& app_copyright(std::ostream& out) {
    out << io::app_version << '\n';

#ifdef GIT_ORIGIN
    out << GIT_ORIGIN << '\n';
#endif

    out << "(c) 2006-2017 Aleks Peshkov\n";

    return out;
}

std::ostream& app_usage(std::ostream& out) {
    return out
        << "Usage: petrel [OPTION]\n"
        << "UCI Chess engine. Plays the game of chess.\n"
        << "\n"
        << "  --help        display this help and exit\n"
        << "  --version     output version information and exit\n"
    ;
}

} //end of namespace io
