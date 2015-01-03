#include "minimaxsearch.h"

namespace Chess {

MinimaxSearch::MinimaxSearch(const Board &position)
    : board(position) {}

void MinimaxSearch::search() {
    foundMoves.clear();
    foundMoves.resize(searchDepth);
    searchMoveCnt = 0;

    foundMovesScore = minimaxSearch(searchDepth);

}

real MinimaxSearch::evaluatePosition()
{
    uint8 pCnt[2][7] = {};

    for (int i=0; i<64; ++i) {
        Piece p = board.piece(i);
        pCnt[p.color()][p.type()]++;
    }

    qreal material =
            //100.0 * (pCnt[Piece::White][Piece::King]   - pCnt[Piece::Black][Piece::King])
            + 9.0 * (pCnt[Piece::White][Piece::Queen]  - pCnt[Piece::Black][Piece::Queen])
            + 5.0 * (pCnt[Piece::White][Piece::Rook]   - pCnt[Piece::Black][Piece::Rook])
            + 3.0 * (pCnt[Piece::White][Piece::Bishop] - pCnt[Piece::Black][Piece::Bishop])
            + 3.0 * (pCnt[Piece::White][Piece::Knight] - pCnt[Piece::Black][Piece::Knight])
            + 1.0 * (pCnt[Piece::White][Piece::Pawn]   - pCnt[Piece::Black][Piece::Pawn]);

    return material;
}

real MinimaxSearch::minimaxSearch(int depth)
{
    if (depth == 0)
        return evaluatePosition();

    Vector<Move> movesList = board.possibleMoves(board.side());
    std::random_shuffle(movesList.begin(), movesList.end()); // randomize the order of the moves

    /* No Valid Moves */
    if (movesList.size() == 0) {
        if (board.isKingAttacked(board.side())) {
            return board.side() == Piece::White ? -1000.0+(searchDepth-depth) : +1000.0-(searchDepth-depth);
        } else {
            return 0.0;     // It's a draw
        }
    }

    float bestValue = (board.side() == Piece::White) ? -INFINITY : +INFINITY;

    for (Move move : movesList) {
        board.make(move);

        float val = minimaxSearch( depth-1 );
        if (board.side() != Piece::White && val > bestValue) {
            bestValue = val;
            foundMoves[searchDepth-depth] = move; // save the best move
        } else if (board.side() != Piece::Black && val < bestValue) {
            bestValue = val;
            foundMoves[searchDepth-depth] = move; // save the  best move
        }

        board.unmake();
        searchMoveCnt++;
    }

    return bestValue;
}

} // !namespace Chess
