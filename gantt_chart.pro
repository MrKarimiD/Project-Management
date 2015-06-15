#-------------------------------------------------
#
# Project created by QtCreator 2015-06-13T12:51:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gantt_chart
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    activity.cpp

HEADERS  += mainwindow.h \
    activity.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../ChartDirector/lib/release/ -lchartdir
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ChartDirector/lib/debug/ -lchartdir
else:unix: LIBS += -L$$PWD/../../ChartDirector/lib/ -lchartdir

INCLUDEPATH += $$PWD/../../ChartDirector/include
DEPENDPATH += $$PWD/../../ChartDirector/include
