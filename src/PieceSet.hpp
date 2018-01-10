#ifndef PIECE_SET_HPP
#define PIECE_SET_HPP

/**
 * class used for enumeration of piece vectors
 */
class PieceSet : public BitSet<PieceSet, Pi> {
public:
    using BitSet::BitSet;

    Pi seekVacant() const {
        _t x = this->_v;
        x = ~x & (x+1); //TRICK: isolate the lowest unset bit
        return PieceSet{x}.index();
    }

    index_t count() const {
        int total = 0;
        for (_t x = this->_v; x; x &= x - 1) {
            total++;
        }
        return total;
    }

    friend io::ostream& operator << (io::ostream& out, PieceSet v) {
        auto flags(out.flags());

        out << std::hex;
        FOR_INDEX(Pi, pi) {
            out << (v[pi]? pi : '.');
        }

        out.flags(flags);
        return out;
    }

};

#endif
