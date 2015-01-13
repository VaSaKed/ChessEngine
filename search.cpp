#include "search.h"

#include "enginetypes.h"
#include "evaluate.h"

namespace Chess {

template<typename T>
Vector<Vector<T> > splitVector(const Vector<T>& vect, int splitTo=2);

Search::Search(unsigned int threadsCount) {

    threads.resize(threadsCount);
    for(int i = 0; i<threadsCount; ++i) {
        threads[i] = new MinimaxSearchThread();
    }
}

SearchResult Search::search(SearchRequest request) {

    // get all possible moves //
    Vector<Move> possibleMoves = request.board.possibleMoves(request.board.side());

    // split the moves between threads //
    Vector<Vector<Move>> threadMoves = splitVector(possibleMoves, threads.size() );


    // start each thread
    for (int i=0; i < threads.size(); ++i){
        SearchRequest threadRequest = request;
        threadRequest.movesFilter = threadMoves[i];
        threads[i]->setSearchRequest(threadRequest);
        threads[i]->start();
    }

    Vector<SearchResult> threadResults(threads.size());
    for (int i=0; i < threads.size(); ++i){
        threadResults[i] = threads[i]->getSearchResult();
    }

    SearchResult result;
    result.request = request;
    result.moveCnt = 0;
    result.score = (request.board.side() == Piece::White) ? -INFINITY : +INFINITY;

    for (int i=0; i < threads.size(); ++i) {
        result.moveCnt += threadResults[i].moveCnt;
        if (request.board.side() == Piece::White && threadResults[i].score > result.score) {
            result.score = threadResults[i].score;
            result.moves = threadResults[i].moves;
        } else if (request.board.side() == Piece::Black && threadResults[i].score < result.score) {
            result.score = threadResults[i].score;
            result.moves = threadResults[i].moves;
        }
    }

    return result;
}

void MinimaxSearchThread::setSearchRequest(const SearchRequest &request)
{
    board = request.board;

    sr.request = request;
    sr.moves.clear();
    sr.moves.resize(sr.request.depth);
    sr.moveCnt = 0;
}

SearchResult MinimaxSearchThread::getSearchResult()
{
    waitForFinish();
    return sr;
}

real MinimaxSearchThread::minimax(int depth)
{
    if (depth == 0)
        return Evaluate::position(board);

    Vector<Move> movesList;
    if (depth == sr.request.depth && sr.request.movesFilter.size() > 0 )
        movesList = sr.request.movesFilter;
    else
        movesList = board.possibleMoves(board.side());

    std::random_shuffle(movesList.begin(), movesList.end()); // randomize the order of the moves

    /* No Valid Moves */
    if (movesList.size() == 0) {
        if (board.isKingAttacked(board.side())) {
            return board.side() == Piece::White ? -1000.0+(sr.request.depth-depth) : +1000.0-(sr.request.depth-depth);
        } else {
            return 0.0;     // It's a draw
        }
    }

    float bestValue = (board.side() == Piece::White) ? -INFINITY : +INFINITY;

    for (Move move : movesList) {
        board.make(move);

        float val = minimax( depth-1 );
        if (board.side() != Piece::White && val > bestValue) {
            bestValue = val;
            sr.moves[sr.request.depth-depth] = move; // save the best move
        } else if (board.side() != Piece::Black && val < bestValue) {
            bestValue = val;
            sr.moves[sr.request.depth-depth] = move; // save the  best move
        }

        board.unmake();
        sr.moveCnt++;
    }

    return bestValue;
}

void MinimaxSearchThread::run()
{
    sr.score = minimax(sr.request.depth);
}



template<typename T>
Vector<Vector<T> > splitVector(const Vector<T>& vect, int splitTo=2) {

    Vector<Vector<T> > result(splitTo);

    int splitSize = vect.size()/splitTo;
    splitSize     = std::max(splitSize, 1);  // check for 0 division;

    for (int i = 0; i < vect.size(); ++i) {
        int whichSplit = (i / splitSize) % splitTo;
        result[whichSplit].push_back(vect[i]);
    }

    return result;
}


} // !namespace Chess
