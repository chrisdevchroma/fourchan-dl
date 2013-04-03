#-------------------------------------------------
#
# Project created by QtCreator 2011-03-20T19:57:54
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = upd4t3r
CONFIG   += console qt
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    filehandler.cpp \
    downloadmanager.cpp \
    applicationinterface.cpp \
    updater.cpp \
    commands.cpp

HEADERS += \
    filehandler.h \
    downloadmanager.h \
    applicationinterface.h \
    updater.h \
    commands.h \
    types.h
#if Q_OS_WIN32
RC_FILE = updater.rc
#elsif Q_OS_MAC
#ICON = resources/fourchan-dl.icns
#endif
RESOURCES += \
    resources.qrc

OTHER_FILES += \
    updater.rc

DEFINES += PROGRAM_VERSION="\\\"1.2\\\""
