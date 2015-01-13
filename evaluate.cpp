#include "evaluate.h"

namespace Chess {

real Evaluate::position(const Board &board)
{
    uint8 pieceCount[2][7] = {};

    for (int i=0; i<64; ++i) {
        Piece p = board.piece(i);
        pieceCount[p.color()][p.type()]++;
    }

    qreal material =
            //100.0 * (pieceCount[Piece::White][Piece::King]   - pieceCount[Piece::Black][Piece::King])
            + 9.0 * (pieceCount[Piece::White][Piece::Queen]  - pieceCount[Piece::Black][Piece::Queen])
            + 5.0 * (pieceCount[Piece::White][Piece::Rook]   - pieceCount[Piece::Black][Piece::Rook])
            + 3.0 * (pieceCount[Piece::White][Piece::Bishop] - pieceCount[Piece::Black][Piece::Bishop])
            + 3.0 * (pieceCount[Piece::White][Piece::Knight] - pieceCount[Piece::Black][Piece::Knight])
            + 1.0 * (pieceCount[Piece::White][Piece::Pawn]   - pieceCount[Piece::Black][Piece::Pawn]);

    return material;
}

} // !namespace Chess
