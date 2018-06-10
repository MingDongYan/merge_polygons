#-------------------------------------------------
#
# Project created by QtCreator 2014-09-07T22:54:09
#
#-------------------------------------------------

QT       += core gui
QT += printsupport
QT +=sql

include($$PWD/../CreateRtsPros.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UnionPolygons
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clipengine.cpp \
    database.cpp \
    file.cpp \
    polygon.cpp \
    ptstoupolygon.cpp \
    mergepolygons.cpp

HEADERS  += mainwindow.h \
    clipengine.h \
    database.h \
    file.h \
    polygon.h \
    ptstoupolygon.h \
    mergepolygons.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Exe/release/ -lRouteManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Exe/debug/ -lRouteManager
else:unix: LIBS += -L$$PWD/../../../Exe/ -lRouteManager

INCLUDEPATH += $$PWD/../RouteManager
DEPENDPATH += $$PWD/../RouteManager

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Exe/release/ -lcdl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Exe/debug/ -lcdl
else:unix: LIBS += -L$$PWD/../../../Exe/ -lcdl

INCLUDEPATH += $$PWD/../cdl
DEPENDPATH += $$PWD/../cdl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Exe/release/ -lJZJTypeManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Exe/debug/ -lJZJTypeManager
else:unix: LIBS += -L$$PWD/../../../Exe/ -lJZJTypeManager

INCLUDEPATH += $$PWD/../JZJTypeManager
DEPENDPATH += $$PWD/../JZJTypeManager

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Exe/release/ -lDbConnect
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Exe/debug/ -lDbConnect
else:unix: LIBS += -L$$PWD/../../../Exe/ -lDbConnect

INCLUDEPATH += $$PWD/../DbConnect
DEPENDPATH += $$PWD/../DbConnect

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Exe/release/ -lAreaManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Exe/debug/ -lAreaManager
else:unix: LIBS += -L$$PWD/../../../Exe/ -lAreaManager

INCLUDEPATH += $$PWD/../AreaManager
DEPENDPATH += $$PWD/../AreaManager
