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
    QObject::connect(eng, SIGNAL(boardChanged(Chess::Board)), uib, SLOT(setBoard(Chess::Board)));

    eng->setBoard(Board::fromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"));

    uib->show();
    return app.exec();
}

