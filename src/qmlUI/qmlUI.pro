QT += qml quick

CONFIG += c++11
TARGET = minsweeper-qml-ui

SOURCES += main.cpp \
    Field.cpp \
    Cells.cpp

RESOURCES += qml.qrc

minsweeper-qml-ui.depends = minsweeper-engine

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Field.hpp \
    Cells.hpp
