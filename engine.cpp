#include "engine.h"

#include <QCoreApplication>
#include <chrono>

namespace Chess {

Engine::Engine(QObject *parent) :
    QObject(parent)
{
}


void Engine::userMoved(Move userMove)
{
    Vector<Move> possibleMoves = board.possibleMoves(board.side());

    Move validMove;
    for (Move move : possibleMoves) {
        if ( move.sameVector(userMove) ) {
            validMove = move;
            break;
        }
    }
    if ( validMove.isValid() ) {
        qDebug() << QString("---Ply #%1---").arg(board.movesDone.size());
        qDebug() << "My Move:" << userMove.origin().file() << userMove.origin().rank() << "to" << userMove.target().file() << userMove.target().rank();
        makeMove(validMove);
    } else if (possibleMoves.size() > 0){
        qDebug() << "Invalid move:" << userMove.origin().file() << userMove.origin().rank() << "to" << userMove.target().file() << userMove.target().rank();
        for (Move move : possibleMoves) {
            qDebug() << "   Valid moves:" << move.origin().file() << move.origin().rank() << "to" << move.target().file() << move.target().rank();
        }
        return;
    } else {
        qDebug() << "Game Over";
    }

    Move minimaxMove;

    //do  {
    qDebug() << QString("---Ply #%1---").arg(board.movesDone.size());
    QCoreApplication::processEvents();
    qDebug() << "AI thinks...";
    SearchRequest request;
    request.board = board;
    request.depth = 5;
    request.movesFilter = Vector<Move>();
    auto start_time = std::chrono::high_resolution_clock::now();
    SearchResult result = minimax.search(request);
    auto stop_time = std::chrono::high_resolution_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    ms = std::max(ms, 1); // prevent the good old divide by 0 problem :)
    real bestMoveScore = result.score;
    minimaxMove = result.moves.size() > 0 ? result.moves[0] : Move();
    int minimaxMoveCnt = result.moveCnt;

    qDebug() << " score:" << bestMoveScore;
    qDebug() << " nodes analized:" << minimaxMoveCnt <<  ms << "ms"
             << "nodes/s =" << (minimaxMoveCnt / ms * 1000);
    qDebug() << "AI Move:" << minimaxMove;
    QCoreApplication::processEvents();

    if (minimaxMove.isValid()) {
        makeMove(minimaxMove);
    } else {
        qDebug() << "Game Over";
    }
    //} while (minimaxMove.isValid());
}

void Engine::makeMove(Move move)
{
    board.make(move);
    emit boardChanged(board);
}

void Engine::setPiece(Coord coord, Piece piece)
{
    board.setPiece(coord, piece);
    emit boardChanged(board);
}

void Engine::setBoard(Board newBoard)
{
    this->board = newBoard;
    emit boardChanged(board);
}

} // !namespace Chess
