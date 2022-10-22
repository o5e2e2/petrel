Developer's notes about source code internals
=============================================

The code created to compile with recent GCC with C++11 features support.
Latest code is not tested with MinGW for Windows, but should be fixed easily.

The engine extensively uses PSHUFB processor instruction that is
available only on SSSE3 instruction set. Porting to ARM NEON should be possible.
Porting to platform without rich SIMD instruction set is possible only with
large performance penalty.

Chess programming code details
------------------------------

*The text below assumes that the reader does know chess programming terminology.*

The engine uses nor mailbox nor bitboard board representation. The fundamental
datasructure is 16 byte vector: vector of bytes for each chess piece of
one of chess playing side. This 16 byte vector perfectly fits into SSE
register of modern x86 (and x64) processors.

The engine incrementally updates attack table using unique data
structure -- matrix of pieces and bitboards. Each bitboard rank stored
separately in one of 8 piece vectors. This allows very fast implemenation
of attackTo() function and relatively fast update of attack table state.
The engine uses so called Reversed BitBoard (aka Hyperbola Quintessense)
piece attack generation of sliding pieces (bishops, rooks, queens).
Legal moves matrix generated directly from the attack matrix.

The engine does not internally distinguish white and black playing sides.
Class "PositionSide" represents a chess side without color specific.
Internally squares are relative to each side's base rank, so all pawns are
pushing from RANK_2 and promoting at RANK_8, both kings at starting position
are set to E1 square and so on.

Abbreviations and conventions used in the source code
-----------------------------------------------------
As convention overloaded "~" operation
used to flip squares, bitbords and other data structures from opposite
side of view to the current one. Flipping operation reverses bytes
order inside bitboard and switches ranks inside squares.

* Bb bb: BitBoard -- bitset of bits for each square of the chessboard
* Pi pi: Piece Index -- one of 16 piece slots in a byte vector; {TheKing = 0} = slot dedicated to the king
* PiBb : matrix of Pi x Bb, used for attack and move tables
* PiVector v: Vector of piece data bytes
* Side side: {My, Op} -- side to move, opposite side
* Color color: {White, Black}
* PieceType ty: {Queen, Rook, Bishop, Knight, Pawn, King} -- colorless kind of chess piece

* typename _t v: internal typename and value used inside generic classes

* operator ~ used for flipping squares and bitboards between sides
* operator %" is a shorcut to "and not" set operation
* operator +, "operator -" for bitsets with assert test that sets are disjoint

Aleks Peshkov (mailto:aleks.peshkov@gmail.com)
