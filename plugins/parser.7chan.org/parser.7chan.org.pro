#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT -= gui

win32:TARGET = ../../parser.7chan.org
else:TARGET = ../parser.7chan.org
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += _LIB_VERSION="\\\"0.3\\\""

SOURCES += parser.7chan.org.cpp

HEADERS += parser.7chan.org.h
