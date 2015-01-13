QMAKE_CXXFLAGS += -std=c++11
QT += gui core widgets

SOURCES += main.cpp \
    board.cpp \
    UI/uiboard.cpp \
    engine.cpp \
    abstractthread.cpp \
    search.cpp \
    evaluate.cpp

HEADERS += \
    board.h \
    UI/uiboard.h \
    enginetypes.h \
    engine.h \
    abstractthread.h \
    search.h \
    evaluate.h

RESOURCES += \
    UI/Images.qrc

OTHER_FILES += \
    release/ChessEngine.exe \
    UI/Images/backup_style_classic.png \
    UI/Images/exp.png \
    UI/Images/screenshot.png \
    UI/Images/style_classic.png \
    release/icudt51.dll \
    release/icuin51.dll \
    release/icuuc51.dll \
    release/libgcc_s_dw2-1.dll \
    release/libstdc++-6.dll \
    release/libwinpthread-1.dll \
    release/Qt5Core.dll \
    release/Qt5Gui.dll \
    release/Qt5Widgets.dll \
    README.md

