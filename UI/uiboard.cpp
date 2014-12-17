#include "uiboard.h"


UIBoard::UIBoard(QWidget *parent)
    : QWidget(parent) {

    initSquares();
    setAcceptDrops(true);
}
