#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT -= gui

win32:TARGET = ../../parserchanarchive
else:TARGET = ../parserchanarchive
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += _LIB_VERSION="\\\"0.3\\\""

SOURCES += parserchanarchive.cpp

HEADERS += parserchanarchive.h
