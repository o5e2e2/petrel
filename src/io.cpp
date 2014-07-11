#include "io.hpp"

void fail_here(std::istream& in) {
    in.setstate(std::ios::failbit);
}

void fail_char(std::istream& in) {
    in.unget();
    fail_here(in);
}

void fail_pos(std::istream& in, std::streampos rewind) {
    in.clear();
    in.seekg(rewind);
    fail_here(in);
}

bool operator == (std::istream& in, const char keyword []) {
    if (keyword == nullptr) { fail_here(in); return false; }

    if (in >> std::ws) {
        auto pos_before = in.tellg();
        auto state_before = in.rdstate();

        for (char c; *keyword != '\0' && in.get(c) && *keyword == c; ++keyword) {}

        if (*keyword == '\0') {
            auto peek = in.peek();
            if (in.eof() || std::isspace(peek)) {
                return true;
            }
        }

        in.clear(state_before);
        in.seekg(pos_before);
    }
    return false;
}
