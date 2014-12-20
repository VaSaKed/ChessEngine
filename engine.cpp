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

void Engine::userMoved(Move move)
{
    bool isValidMove = false;
    Vector<Move> posmovs = board.possibleMoves(board.side());
    if (posmovs.size() == 0 ) {
        qDebug() << "End of the game";
        return;
    }

    for (Move validMove : posmovs) {
        if (move.source() == validMove.source() && move.target() == validMove.target() ) isValidMove = true;
    }
    if ( isValidMove ) {
        makeMove(move);
        qDebug() << QString("---Move #%1---").arg(board.movesDone.size());
        qDebug() << "My Move:" << move.source().file() << move.source().rank() << "to" << move.target().file() << move.target().rank();
    } else {
        qDebug() << "Invalid move:" << move.source().file() << move.source().rank() << "to" << move.target().file() << move.target().rank();
        for (Move move : posmovs) {
            qDebug() << "   Valid moves:" << move.source().file() << move.source().rank() << "to" << move.target().file() << move.target().rank();
        }
        return;
    }
    QCoreApplication::processEvents();
    think(4, Piece::Black);
    QCoreApplication::processEvents();

    qDebug() << "AI Move:" << minimaxMove.source().file() << minimaxMove.source().rank()
             << "to" << minimaxMove.target().file() << minimaxMove.target().rank()
             << QString("(Analized %1 moves)").arg(minimaxMoveCnt);

    qDebug() << QString("-------------");
    if (minimaxMove.isValid()) {
        makeMove(minimaxMove);
    } else {
        qDebug() << "End of the Game";
    }
}

void Engine::think(int depth, Piece::Color maximazingPlayer)
{
    minimaxDepth = depth;
    minimaxMoveCnt = 0;
    qDebug() << "AI Score:" << minimax(depth, maximazingPlayer);
}

void Engine::makeMove(Move move)
{
    board.make(move);
    emit squareChanged(move.source(), Piece());
    emit squareChanged(move.target(), move.isPromotion() ? move.promotedPiece() : move.sourcePiece() );
}

void Engine::setPiece(Coord coord, Piece piece)
{
    board.setPiece(coord, piece);
    emit squareChanged(coord, piece);
}

} // !namespace Chess
