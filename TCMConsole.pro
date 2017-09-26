#-------------------------------------------------
#
# Project created by QtCreator 2017-07-05T14:38:36
#
#-------------------------------------------------

QT       += core network serialport

QT       -= gui

TARGET = TCMConsole
CONFIG   += console
CONFIG   -= app_bundle
DEFINES += QT_NO_DEBUG_OUTPUT

TEMPLATE = app


SOURCES += main.cpp \
        qpiconfig.cpp \
        serialporttransmitter.cpp \
        consolecontrol.cpp \
        tcm_ethernet.cpp

HEADERS +=protocol_TCM2.h \
        qpiconfig.h \
        serialporttransmitter.h \
        consolecontrol.h \
        tcm_ethernet.h

