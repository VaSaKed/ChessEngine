#ifndef SEARCH_H
#define SEARCH_H

#include "enginetypes.h"
#include "board.h"
#include "abstractthread.h"

namespace Chess {

struct SearchRequest {
    Board board;
    int depth;
    Vector<Move> movesFilter;
};

struct SearchResult {
    SearchRequest request;
    Vector<Move> moves;
    real score;
    int moveCnt;
};

class MinimaxSearchThread : public AbstractThread {

    Board board;
    SearchResult sr;

public:

    void setSearchRequest(const SearchRequest& request);
    SearchResult getSearchResult();

private:

    real minimax(int depth);

protected:

    void run() override;
}; // !class MinimaxSearchThread


class Search
{
    Vector<MinimaxSearchThread*> threads;

public:

    Search(unsigned int threadsCount = std::thread::hardware_concurrency());

    SearchResult search(SearchRequest request);

}; // !class Search

} //!namespace chess

#endif // SEARCH_H
