#ifndef MINIMAXSEARCH_H
#define MINIMAXSEARCH_H

#include "enginetypes.h"
#include "board.h"

#include <algorithm> // random_shuffle()

namespace Chess {

class MinimaxSearch
{
public: // !!!
    Board board;
    Vector<Move> foundMoves;
    real foundMovesScore;

    int searchDepth;
    int searchMoveCnt;


public:
    MinimaxSearch(const Board & position = Board());

    void setPostion(const Board & position) {
        board = position;
    }

    inline void setDepth(int depth) {
        searchDepth = depth;
    }

    void search();

    real evaluatePosition();

private:
    real minimaxSearch(int depth);
};

} //!namespace chess

#endif // MINIMAXSEARCH_H
