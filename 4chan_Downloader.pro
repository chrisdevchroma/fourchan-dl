#-------------------------------------------------
#
# Project created by QtCreator 2011-02-06T19:46:34
#
#-------------------------------------------------

QT       += core gui network

TARGET = 4chan_Downloader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    parser.cpp \
    ui4chan.cpp

HEADERS  += mainwindow.h \
    parser.h \
    ui4chan.h

FORMS    += mainwindow.ui \
    ui4chan.ui

RESOURCES += \
    resources.qrc
