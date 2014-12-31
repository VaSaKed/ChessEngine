#include "engine.h"
#include <QCoreApplication>

#include <algorithm>
#include <chrono>

namespace Chess {

Engine::Engine(QObject *parent) :
    QObject(parent)
{
}

float Engine::evaluatePosition()
{
    uint8 pCnt[2][7] = {};

    for (int i=0; i<64; ++i) {
        Piece p = board.piece(i);
        pCnt[p.color()][p.type()]++;
    }

    qreal weight =
            //100.0 * (pCnt[Piece::White][Piece::King]   - pCnt[Piece::Black][Piece::King])
            + 9.0 * (pCnt[Piece::White][Piece::Queen]  - pCnt[Piece::Black][Piece::Queen])
            + 5.0 * (pCnt[Piece::White][Piece::Rook]   - pCnt[Piece::Black][Piece::Rook])
            + 3.0 * (pCnt[Piece::White][Piece::Bishop] - pCnt[Piece::Black][Piece::Bishop])
            + 3.0 * (pCnt[Piece::White][Piece::Knight] - pCnt[Piece::Black][Piece::Knight])
            + 1.0 * (pCnt[Piece::White][Piece::Pawn]   - pCnt[Piece::Black][Piece::Pawn]);

    return weight;
}

float Engine::minimax(int depth)
{
    minimaxMoveCnt++;
    if (depth == 0)
        return evaluatePosition();


    Vector<Move> movesList = board.possibleMoves(board.side());
    std::random_shuffle(movesList.begin(), movesList.end()); // randomize the order of moves

    /* No Valid Moves */
    if (movesList.size() == 0) {
        if (board.isKingAttacked(board.side())) {
            return board.side() == Piece::White ? -1000.0+(minimaxDepth-depth) : +1000.0-(minimaxDepth-depth);
        } else {
            return 0.0;     // It's a draw
        }
    }

    float bestValue;

    if (board.side() == Piece::White) {
        bestValue = -INFINITY;
        for (Move move : movesList) {
            board.make(move);
            float val = minimax( depth-1 );
            if (val > bestValue){
                bestValue = val;
                if (depth == minimaxDepth)
                    minimaxMove = move;
            }
            board.unmake();
        }
    } else {
        bestValue = +INFINITY;

        for (Move move : movesList) {
            board.make(move);
            float val = minimax( depth-1 );
            if (val < bestValue) {
                bestValue = val;
                if (depth == minimaxDepth)
                    minimaxMove = move;
            }
            board.unmake();

        }
    }
    return bestValue;
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
    do  {
    qDebug() << QString("---Ply #%1---").arg(board.movesDone.size());
    QCoreApplication::processEvents();
    qDebug() << "AI thinks...";
    think(4);
    QCoreApplication::processEvents();

    if (minimaxMove.isValid()) {
        makeMove(minimaxMove);
    } else {
        qDebug() << "Game Over";
    }
    } while (minimaxMove.isValid());
}

void Engine::think(int depth)
{
    minimaxMove = Move();
    minimaxDepth = depth;
    minimaxMoveCnt = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    real bestMoveScore = minimax(depth);
    auto stop_time = std::chrono::high_resolution_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
    ms = std::max(ms, 1); // prevent the good old divide by 0 problem :)

    qDebug() << " score:" << bestMoveScore;
    qDebug() << " nodes analized:" << minimaxMoveCnt <<  ms << "ms"
             << "nodes/s =" << (minimaxMoveCnt / ms * 1000);
    qDebug() << "AI Move:" << minimaxMove.origin().file() << minimaxMove.origin().rank()
             << "to" << minimaxMove.target().file() << minimaxMove.target().rank();
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
