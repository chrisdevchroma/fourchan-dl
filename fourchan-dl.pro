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
    updater/updater.cpp \
    updater/main.cpp \
    updater/filehandler.cpp \
    updater/downloadmanager.cpp \
    updater/commands.cpp \
    updater/applicationinterface.cpp

HEADERS  += mainwindow.h \
    parser.h \
    ui4chan.h \
    uiinfo.h \
    uiconfig.h \
    thumbnailthread.h \
    uilisteditor.h \
    applicationupdateinterface.h \
    updater/commands.h \
    defines.h \
    updater/updater.h \
    updater/types.h \
    updater/filehandler.h \
    updater/downloadmanager.h \
    updater/commands.h \
    updater/applicationinterface.h

FORMS    += mainwindow.ui \
    ui4chan.ui \
    uiinfo.ui \
    uiconfig.ui \
    uilisteditor.ui

RESOURCES += \
    resources.qrc \
    updater/resources.qrc
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
    fourchan-dl.rc \
    updater/updater.rc \
    updater/updater.pro.user \
    updater/updater.pro \
    updater/update.ico
