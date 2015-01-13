#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "board.h"
#include "search.h"


namespace Chess{

class Engine : public QObject
{
    Q_OBJECT

public:
    Board board;
    Search minimax;

public:
    explicit Engine(QObject *parent = 0);

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
