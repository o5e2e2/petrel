#include "io.hpp"

namespace io {
    void fail_here(std::istream& in) {
        in.setstate(std::ios::failbit);
    }

    void fail_char(std::istream& in) {
        in.unget();
        io::fail_here(in);
    }

    void fail_pos(std::istream& in, std::streampos rewind) {
        in.clear();
        in.seekg(rewind);
        io::fail_here(in);
    }

    void fail_rewind(std::istream& in) {
        io::fail_pos(in, 0);
    }

    /**
     * Tries to read the next token and compare it with the given keyword
     * Return:
     * true  -- next token is equals to keyword, shift stream cursor past the token
     * false -- failed to read or to match next token with the given keyword, stream state is not changed
     **/
    bool next(std::istream& in, io::literal keyword) {
        if (keyword == nullptr || *keyword == '\0' || !(in >> std::ws)) {
            return false;
        }

        auto pos_before = in.tellg();
        auto state_before = in.rdstate();

        //compare each keyword char with each token char
        for (; *keyword != '\0'; ++keyword) {
            if (in.get() == *keyword) { continue; }

            goto mismatch;
        }

        //test that token size is equal to keyword size
        {
            auto peek = in.peek();
            if (in.eof() || std::isspace(peek)) {
                return true;
            }
        }

        mismatch:
            in.clear(state_before);
            in.seekg(pos_before);
            return false;
    }

}
