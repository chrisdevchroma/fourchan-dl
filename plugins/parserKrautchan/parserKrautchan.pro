#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = ../../parserKrautchan
else:TARGET = ../parserKrautchan
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += KRAUTCHANPARSER_LIBRARY _LIB_VERSION="\\\"0.9\\\""

SOURCES += parserKrautchan.cpp

HEADERS += parserKrautchan.h
