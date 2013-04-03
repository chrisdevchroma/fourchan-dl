#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = ../../parser4chan
else:TARGET = ../parser4chan
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += FOURCHANPARSER_LIBRARY _LIB_VERSION="\\\"1.7\\\""

SOURCES += parser4chan.cpp

HEADERS += parser4chan.h
