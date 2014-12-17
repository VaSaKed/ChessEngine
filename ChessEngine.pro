QMAKE_CXXFLAGS += -std=c++11
QT += gui core widgets

SOURCES += main.cpp \
    board.cpp \
    UI/uiboard.cpp \
    engine.cpp

HEADERS += \
    board.h \
    UI/uiboard.h \
    enginetypes.h \
    engine.h

RESOURCES += \
    UI/Images.qrc

