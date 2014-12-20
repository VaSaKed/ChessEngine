#include <board.h>
#include <QDebug>
#include <QApplication>
#include <UI/uiboard.h>
#include <QPixmap>
#include "engine.h"
using namespace Chess;

int main(int argc, char*argv[])
{
    QApplication app(argc, argv);


    UIBoard *uib = new UIBoard();
    Engine  *eng = new Engine();

    QObject::connect(uib, SIGNAL(userMoved(Chess::Move)), eng, SLOT(userMoved(Chess::Move)));
    QObject::connect(eng, SIGNAL(squareChanged(Chess::Coord,Chess::Piece)), uib, SLOT(setPiece(Chess::Coord,Chess::Piece)));

    for(int i = 0; i < 8; ++i){
        eng->setPiece(Coord(i,1), Piece(Piece::Pawn, Piece::White));
        eng->setPiece(Coord(i,6), Piece(Piece::Pawn, Piece::Black));
//        switch (i) {
//        case 0: case 7:
//            eng->setPiece(Coord(i,0), Piece(Piece::Rook, Piece::White));
//            eng->setPiece(Coord(i,7), Piece(Piece::Rook, Piece::Black));
//            break;
//        case 1: case 6:
//            eng->setPiece(Coord(i,0), Piece(Piece::Knight, Piece::White));
//            eng->setPiece(Coord(i,7), Piece(Piece::Knight, Piece::Black));
//            break;
//        case 2: case 5:
//            eng->setPiece(Coord(i,0), Piece(Piece::Bishop, Piece::White));
//            eng->setPiece(Coord(i,7), Piece(Piece::Bishop, Piece::Black));
//            break;
//        case 3:
//            eng->setPiece(Coord(i,0), Piece(Piece::Queen, Piece::White));
//            eng->setPiece(Coord(i,7), Piece(Piece::Queen, Piece::Black));
//            break;
//        case 4:
//            eng->setPiece(Coord(i,0), Piece(Piece::King, Piece::White));
//            eng->setPiece(Coord(i,7), Piece(Piece::King, Piece::Black));
//            break;
//        }
    }

    uib->show();
    return app.exec();
}

