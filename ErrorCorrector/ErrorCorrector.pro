#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T17:03:03
#
#-------------------------------------------------

QT       += core
QT       += xml

QT       -= gui

TARGET = ErrorCorrector
CONFIG   += console
CONFIG += build_all
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    application.cpp \
    dotfilenode.cpp \
    editdistance.cpp \
    icginfonode.cpp \
    iohandler.cpp \
    scusitree.cpp \
    sspoutpputnode.cpp \
    textinfonode.cpp

HEADERS += \
    application.h \
    definitions.h \
    dotfilenode.h \
    editdistance.h \
    icginfonode.h \
    iohandler.h \
    scusitree.h \
    sspoutpputnode.h \
    textinfonode.h \
    auxiliaryoriginalwordcontainer.h
