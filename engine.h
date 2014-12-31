#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "board.h"


namespace Chess{

class Engine : public QObject
{
    Q_OBJECT

public:
    Board board;


    Move minimaxMove;
    int minimaxMoveCnt;
    int minimaxDepth;

public:
    explicit Engine(QObject *parent = 0);

    void think(int depth);

    float evaluatePosition();

    float minimax(int depth);

public slots:
    void userMoved(Chess::Move userMove);
    void makeMove(Chess::Move move);
    void setPiece(Chess::Coord coord, Chess::Piece piece);
    void setBoard(Board newBoard);

signals:
    void boardChanged(Chess::Board board);

};

} // !namespace Chess

#endif // ENGINE_H
