#-------------------------------------------------
#
# Project created by QtCreator 2011-02-06T19:46:34
#
#-------------------------------------------------

QT       += core gui network

TARGET = fourchan-dl
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    parser.cpp \
    ui4chan.cpp \
    uiinfo.cpp \
    uiconfig.cpp \
    thumbnailthread.cpp \
    uilisteditor.cpp \
    applicationupdateinterface.cpp \
    updater/commands.cpp \
    blacklist.cpp

HEADERS  += mainwindow.h \
    parser.h \
    ui4chan.h \
    uiinfo.h \
    uiconfig.h \
    thumbnailthread.h \
    uilisteditor.h \
    defines.h \
    applicationupdateinterface.h \
    updater/commands.h \
    blacklist.h

FORMS    += mainwindow.ui \
    ui4chan.ui \
    uiinfo.ui \
    uiconfig.ui \
    uilisteditor.ui

RESOURCES += \
    resources.qrc

win32: RC_FILE = fourchan-dl-win.rc
os2: RC_FILE = fourchan-dl-os2.rc
macx: ICON = resources/fourchan-dl.icns

BUILDDATE = __DATE__
DEFINES += BUILDDATE=$${BUILDDATE} PROGRAM_VERSION="\\\"0.9.7\\\""

OTHER_FILES += \
    readme.txt \
    settings.ini \
    fourchan-dl.rc
