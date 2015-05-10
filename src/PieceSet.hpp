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

};

#endif
