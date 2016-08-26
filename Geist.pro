#-------------------------------------------------
#
# Project created by QtCreator 2013-09-15T09:57:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Geist
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    conversion.cpp \
    highlighter.cpp \
    templates.cpp \
    snippets.cpp \
    files.cpp \
    runner.cpp

HEADERS  += mainwindow.h \
    conversion.h \
    highlighter.h \
    search.h \
    templates.h \
    snippets.h \
    files.h \
    runner.h

FORMS    += mainwindow.ui

OTHER_FILES +=
