#ifndef UIBOARD_H
#define UIBOARD_H

#include <QLabel>
#include <QPalette>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QDebug>

#include "board.h"

class UIBoard : public QWidget {

    Q_OBJECT

public:
    explicit UIBoard(QWidget *parent = 0);

    Chess::Piece getPiece(Chess::Coord coord);
    void setPiece(Chess::Coord coord, Chess::Piece piece);

public slots:
    void setBoard(Chess::Board board);

signals:
    void userMoved(Chess::Move move);

protected:
    void resizeEvent(QResizeEvent * event) override;

private:
    void resizeSquares();

    void updateSquare(int x, int y);

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void mousePressEvent(QMouseEvent *event);


private:
    QLabel *squares[8][8];
    Chess::Board uBoard;
    QSize squareSize;
};

#endif // UIBOARD_H
