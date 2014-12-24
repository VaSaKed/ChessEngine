#include "engine.h"
#include <QCoreApplication>

#include <algorithm>

namespace Chess {

Engine::Engine(QObject *parent) :
    QObject(parent)
{
}

float Engine::evaluatePosition(Piece::Color forSide)
{
    uint8 pCnt[2][7] = {};

    for (int i=0; i<64; ++i) {
        Piece p = board.piece(i);
        pCnt[p.color()][p.type()]++;
    }

    qreal weight =
            100.0 * (pCnt[Piece::White][Piece::King]   - pCnt[Piece::Black][Piece::King])
            + 9.0 * (pCnt[Piece::White][Piece::Queen]  - pCnt[Piece::Black][Piece::Queen])
            + 5.0 * (pCnt[Piece::White][Piece::Rook]   - pCnt[Piece::Black][Piece::Rook])
            + 3.0 * (pCnt[Piece::White][Piece::Bishop] - pCnt[Piece::Black][Piece::Bishop])
            + 3.0 * (pCnt[Piece::White][Piece::Knight] - pCnt[Piece::Black][Piece::Knight])
            + 1.0 * (pCnt[Piece::White][Piece::Pawn]   - pCnt[Piece::Black][Piece::Pawn]);

    weight = (forSide == Piece::White) ? weight : -weight;

    return weight;
}

float Engine::minimax(int depth, Piece::Color maximazingPlayer)
{
    minimaxMoveCnt++;
    if (depth == 0)
        return evaluatePosition(maximazingPlayer);

    /* No Valid Moves */
    Vector<Move> movesList = board.possibleMoves(board.side());
    if (movesList.size() == 0)
        return evaluatePosition(maximazingPlayer);

    std::random_shuffle(movesList.begin(), movesList.end()); // randomize the order of moves

    float bestValue = 0.0;
    if (board.side() == maximazingPlayer) {
        bestValue = -INFINITY;
        for (Move move : movesList) {
            board.make(move);
            float val = minimax( depth-1, maximazingPlayer );
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
            float val = minimax( depth-1, maximazingPlayer );
            if (val < bestValue)
                bestValue = val;
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
        makeMove(validMove);
        qDebug() << QString("---Move #%1---").arg(board.movesDone.size());
        qDebug() << "My Move:" << userMove.origin().file() << userMove.origin().rank() << "to" << userMove.target().file() << userMove.target().rank();
    } else if (possibleMoves.size() > 0){
        qDebug() << "Invalid move:" << userMove.origin().file() << userMove.origin().rank() << "to" << userMove.target().file() << userMove.target().rank();
        for (Move move : possibleMoves) {
            qDebug() << "   Valid moves:" << move.origin().file() << move.origin().rank() << "to" << move.target().file() << move.target().rank();
        }
        return;
    } else {
        qDebug() << "Game Over";
    }

    QCoreApplication::processEvents();
    think(4, Piece::Black);
    QCoreApplication::processEvents();

    qDebug() << "AI Move:" << minimaxMove.origin().file() << minimaxMove.origin().rank()
             << "to" << minimaxMove.target().file() << minimaxMove.target().rank()
             << QString("(Analized %1 moves)").arg(minimaxMoveCnt);

    qDebug() << QString("-------------");
    if (minimaxMove.isValid()) {
        makeMove(minimaxMove);
    } else {
        qDebug() << "Game Over";
    }
}

void Engine::think(int depth, Piece::Color maximazingPlayer)
{
    minimaxMove = Move();
    minimaxDepth = depth;
    minimaxMoveCnt = 0;
    qDebug() << "AI Score:" << minimax(depth, maximazingPlayer);
}

void Engine::makeMove(Move move)
{
    board.make(move);
    emit squareChanged(move.origin(), Piece());
    emit squareChanged(move.target(), move.isPromotion() ? move.promotedPiece() : move.originPiece() );
}

void Engine::setPiece(Coord coord, Piece piece)
{
    board.setPiece(coord, piece);
    emit squareChanged(coord, piece);
}

} // !namespace Chess
