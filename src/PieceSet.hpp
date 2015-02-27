#ifndef PIECE_SET_HPP
#define PIECE_SET_HPP

/**
 * class used for enumeration of piece vectors
 */
class PieceSet : public BitSet<PieceSet, Pi, unsigned> {
public:
    using BitSet::BitSet;

    Pi seekVacant() const {
        _t x = this->_v;
        x = ~x & (x+1); //TRICK: isolate the lowest unset bit
        return PieceSet{x}.index();
    }

    //for debugging
    friend std::ostream& operator << (std::ostream& out, PieceSet v) {
        auto flags(out.flags());

        out << std::hex;
        FOR_INDEX(Pi, pi) {
            if (v[pi]) {
                out << pi;
            }
            else {
                out << ".";
            }
        }

        out.flags(flags);
        return out;
    }

};

#endif
