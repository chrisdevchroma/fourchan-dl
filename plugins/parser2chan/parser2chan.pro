#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = ../../parser2chan
else:TARGET = ../parser2chan
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += PARSER2CHAN_LIBRARY _LIB_VERSION="\\\"0.4\\\""

SOURCES += parser2chan.cpp

HEADERS += parser2chan.h
