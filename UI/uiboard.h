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

    Chess::Piece getPiece(Chess::Coord coord) {
        return uBoard.piece(coord);
    }
public slots:
    void setPiece(Chess::Coord coord, Chess::Piece piece) {
        uBoard.setPiece(coord, piece);
        updateSquare(coord.file(), 7-coord.rank());
    }

protected:
    void resizeEvent(QResizeEvent * event) override {
        resizeSquares();
        QWidget::resizeEvent(event);
    }
private:
    void initSquares(){
        bool whiteSquare = true;
        for (int y = 0; y<8; ++y, whiteSquare = !whiteSquare){
            for (int x = 0; x<8; ++x, whiteSquare = !whiteSquare) {
                squares[y][x] = new QLabel(this);
                squares[y][x]->setAlignment(Qt::AlignCenter);
                squares[y][x]->setAutoFillBackground(true);
                QColor squareColor = whiteSquare ? QColor(241,210,169)
                                                 : QColor(186,131,89);
                QPalette p(this->palette());
                p.setColor(QPalette::Window, squareColor);
                squares[y][x]->setPalette(p);
            }
        }
        resizeSquares();
    }

    void resizeSquares(){
        int newSize = size().width() < size().height() ? size().width()  / 8
                                                       : size().height() / 8;
        squareSize = QSize(newSize, newSize);

        for (int y = 0; y<8; ++y){
            for (int x = 0; x<8; ++x) {
                updateSquare(x, y);
            }
        }
    }

    void updateSquare(int x, int y) {

        if (squares[y][x]->size() != squareSize) {
            squares[y][x]->resize(squareSize);
            squares[y][x]->move(x * squareSize.width(), y * squareSize.height());
        }

        Chess::Piece piece = getPiece(Chess::Coord(x, 7-y));

        QPixmap image = QPixmap(":/Images/style_classic.png");

        QSize imgSize(image.width()/6, image.height()/2);

        QPoint imgOfst;
        imgOfst.ry() = imgSize.height() * !piece.color();

        switch (piece.type()){
        case Chess::Piece::Pawn:
            imgOfst.rx() = 5 * imgSize.width(); break;
        case Chess::Piece::Knight:
            imgOfst.rx() = 4 * imgSize.width(); break;
        case Chess::Piece::Bishop:
            imgOfst.rx() = 1 * imgSize.width(); break;
        case Chess::Piece::Rook:
            imgOfst.rx() = 0 * imgSize.width(); break;
        case Chess::Piece::Queen:
            imgOfst.rx() = 2 * imgSize.width(); break;
        case Chess::Piece::King:
            imgOfst.rx() = 3 * imgSize.width(); break;
        case Chess::Piece::Empty:
            image = QPixmap(); break;
        }

        image = image.copy(QRect(imgOfst, imgSize));

        if (!image.isNull())
            image = image.scaled(squareSize,Qt::KeepAspectRatio, Qt::SmoothTransformation);
        squares[y][x]->setPixmap(image);
    }

    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-dnditemdata"))
            event->acceptProposedAction();
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            int srcFile;
            int srcRank;

            dataStream >> srcFile >> srcRank;

            int trgtFile = event->pos().x() / squareSize.width();
            int trgtRank = 7 - event->pos().y() / squareSize.width();

            //Chess::Piece src  = uBoard.piece(Chess::Coord(srcFile, srcRank));
            //Chess::Piece trgt = uBoard.piece(Chess::Coord(trgtFile, trgtRank));

            emit userMoved(Chess::Move(Chess::Coord(srcFile, srcRank),
                                       Chess::Coord(trgtFile, trgtRank) ));

            event->accept();
        } else {
            event->ignore();
        }
    }

    void mousePressEvent(QMouseEvent *event){

        QLabel *square = static_cast<QLabel*>(childAt(event->pos()));
        int srcFile = event->pos().x()/squareSize.width();
        int srcRank = 7 - event->pos().y()/squareSize.width();

        if (!uBoard.isOccupied(Chess::Coord(srcFile, srcRank))) {
            event->ignore();
            return;
        }

        QPixmap pixmap = *square->pixmap();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << srcFile << srcRank;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos() - square->pos());

        QPixmap tempPixmap = pixmap;
        QPainter painter;
        painter.begin(&tempPixmap);
        painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
        painter.end();

        square->setPixmap(tempPixmap);
        Chess::Vector<Chess::Move> possibleMoves = uBoard.possibleMoves(Chess::Coord(srcFile,srcRank));
        for(Chess::Move move : possibleMoves) {
            int y = 7 - move.target().rank();
            int x = move.target().file();

            QPixmap result = squares[y][x]->pixmap() ? *squares[y][x]->pixmap() : QPixmap();
            if (result.isNull()){
                result = QPixmap(squareSize);
                result.fill(QColor(0,0,0,0));
            }
            QPainter p;
            p.begin(&result);
            p.setBrush(QBrush(QColor(250, 250, 150, 200)));
            p.setPen(QColor(150, 200, 150, 200));
            p.drawEllipse(result.rect().center(), result.width()/10, result.height()/10);
            p.end();

            squares[y][x]->setPixmap(result);
        }
        drag->exec(Qt::MoveAction);
        updateSquare(srcFile, 7-srcRank);

        for(Chess::Move move : possibleMoves) {
            int y = 7 - move.target().rank();
            int x = move.target().file();
            updateSquare(x,y);
        }
    }
signals:
    void userMoved(Chess::Move move);

private:
    QLabel *squares[8][8];
    Chess::Board uBoard;
    QSize squareSize;
};

#endif // UIBOARD_H
