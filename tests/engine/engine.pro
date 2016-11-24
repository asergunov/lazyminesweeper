TEMPLATE = app

CONFIG -= QT

SOURCES +=  main.cpp \
            SquareBoard.cpp \
    SquareBoardData.cpp \
    Solver.cpp \
    Equations.cpp


LIBS += -L/usr/local/lib -lgtest

INCLUDEPATH += $$PWD/../../src/engine

