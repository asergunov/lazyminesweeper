TEMPLATE = app

CONFIG -= QT

SOURCES +=  main.cpp \
            SquareBoard.cpp


LIBS += -L/usr/local/lib -lgtest

INCLUDEPATH += $$PWD/../../src/engine

