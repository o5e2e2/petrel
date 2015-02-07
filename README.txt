Developer's notes about source code internals
(partially outdated as of 20140423)

The text below assumes that the reader does know chess programming terminology.

The source code written in C++11 and developed with recent GCC compiler.

The engine uses nor mailbox nor bitboard board representation. The fundamental
datasructure is 16 byte vector: vector of bytes for each
chess piece of one of chess playing side. This 16 byte vector perfectly
fits into SSE register of modern x86 (and x64) processors.

Unfortunately this means that porting the code for processors lucking
SIMD instruction support is not practical for performance reasons.
The engine extensively uses PSHUFB processor instruction that is
available only on SSSE3 instruction set. Porting to ARM NEON is possible.

The engine incrementally updates attack table using unique data
structure -- matrix of pieces and bitboards. Each bitboard rank stored
separately in one of 8 piece vectors. This allows very fast implemenation
of attackTo() function and relatively fast update of attack table state.
The engine uses so called Reversed BitBoard (aka Hyperbola Quintessense)
piece attack generation of sliding pieces (bishops, rooks, queens).

Because of complex attack tables the engine uses copy-make approach.
Legal moves matrix are generated directly from the attack matrix.

The engine does not internally distinguish white and black playing sides.
Class "PositionSide" represents a chess side without color specific.
Internally squares are relative to each side's base rank, so all pawns are
pushing from RANK_2 and promoting at RANK_8, kings at starting position
are set at E1 square and so on. As convention overloaded "~" operation
used to flip squares, bitbords and other data structures from opposite
side of view to the current one. Flipping operation reverses bytes
order inside bitboard and switches ranks inside squares.

Some abbreviations and conventions in the source code.

class Bb (BitBoard) -- bitset of bits for each square of the chessboard
class Pi (Piece Index) -- one of 16 piece slots in a vector
class VectorPi -- 16 byte long vector.

typename _t -- internal value type used inside class templates
_v -- actual internal value of "_t" type
typename Self -- the name of the current template class
typename Self::Arg -- function argument type (either "Self" or "const Self&")

"operator ~" used for flipping squares and bitboards between sides
"operator >>" and "operator <<" used to enumerate bits in bitset
"operator %" is shorcut to "and not" set operation
"operator +", "operator -" for bitsets with assert test that sets are disjoint

Aleks Peshkov (mailto:aleks.peshkov@gmail.com)
