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
    updater/commands.cpp

HEADERS  += mainwindow.h \
    parser.h \
    ui4chan.h \
    uiinfo.h \
    uiconfig.h \
    thumbnailthread.h \
    uilisteditor.h \
    defines.h \
    applicationupdateinterface.h \
    updater/commands.h

FORMS    += mainwindow.ui \
    ui4chan.ui \
    uiinfo.ui \
    uiconfig.ui \
    uilisteditor.ui

RESOURCES += \
    resources.qrc
#if Q_OS_WIN32
RC_FILE = fourchan-dl.rc
#elsif Q_OS_MAC
ICON = resources/fourchan-dl.icns
#endif

BUILDDATE = __DATE__
DEFINES += BUILDDATE=$${BUILDDATE} PROGRAM_VERSION="\\\"0.0.0\\\""

OTHER_FILES += \
    readme.txt \
    settings.ini \
    fourchan-dl.rc
