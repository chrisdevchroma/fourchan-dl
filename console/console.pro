#-------------------------------------------------
#
# Project created by QtCreator 2011-02-06T19:46:34
#
#-------------------------------------------------

QT += core network
QT -= gui widgets

win32:TARGET = ../../fourchan-dl-console
else:TARGET = ../fourchan-dl-console

TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp \
    threadhandler.cpp \
    imagethread.cpp \
    ../gui/thumbnailremover.cpp \
    ../gui/supervisednetworkreply.cpp \
    ../gui/requesthandler.cpp \
    ../gui/pluginmanager.cpp \
    ../gui/ecwin7.cpp \
    ../gui/downloadrequest.cpp \
    ../gui/downloadmanager.cpp \
    ../gui/blacklist.cpp \
    ../gui/applicationupdateinterface.cpp \
    ../gui/networkaccessmanager.cpp \
    ../updater/commands.cpp

RESOURCES +=

include(../gui/QsLog.pri)

win32: RC_FILE = fourchan-dl-win.rc
#os2: RC_FILE = fourchan-dl-os2.rc
#macx: ICON = resources/fourchan-dl.icns

BUILDDATE = __DATE__
VERSION = 0.1.0
DEFINES += BUILDDATE=$${BUILDDATE} PROGRAM_VERSION="\\\"$${VERSION}\\\""

win32: LIBS += libole32

OTHER_FILES += \ 
    fourchan-dl-win.rc \
    fourchan-dl-os2.rc \
    fourchan-dl.rc

HEADERS += \
    ../gui/thumbnailremover.h \
    ../gui/supervisednetworkreply.h \
    ../gui/structs.h \
    ../gui/requesthandler.h \
    ../gui/pluginmanager.h \
    ../gui/ParserPluginInterface.h \
    ../gui/ecwin7.h \
    ../gui/downloadrequest.h \
    ../gui/downloadmanager.h \
    ../gui/defines.h \
    ../gui/blacklist.h \
    ../gui/applicationupdateinterface.h \
    threadhandler.h \
    imagethread.h \
    ../gui/networkaccessmanager.h \
    ../updater/commands.h
