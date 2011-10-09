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
    uiimageoverview.cpp \
    uiinfo.cpp \
    uiconfig.cpp \
    thumbnailthread.cpp \
    uilisteditor.cpp \
    applicationupdateinterface.cpp \
    updater/commands.cpp \
    blacklist.cpp \
    thumbnailremoverthread.cpp \
    downloadmanager.cpp \
    requesthandler.cpp \
    supervisednetworkreply.cpp \
    downloadrequest.cpp \
    uithreadadder.cpp \
    dialogfoldershortcut.cpp \
    foldershortcuts.cpp \
    thumbnailview.cpp \
    networkaccessmanager.cpp \
    pluginmanager.cpp

HEADERS  += mainwindow.h \
    uiimageoverview.h \
    uiinfo.h \
    uiconfig.h \
    thumbnailthread.h \
    uilisteditor.h \
    defines.h \
    applicationupdateinterface.h \
    updater/commands.h \
    blacklist.h \
    thumbnailremoverthread.h \
    downloadmanager.h \
    requesthandler.h \
    supervisednetworkreply.h \
    downloadrequest.h \
    uithreadadder.h \
    dialogfoldershortcut.h \
    foldershortcuts.h \
    thumbnailview.h \
    networkaccessmanager.h \
    pluginmanager.h \
    ParserPluginInterface.h \
    structs.h

FORMS    += mainwindow.ui \
    uiimageoverview.ui \
    uiinfo.ui \
    uiconfig.ui \
    uilisteditor.ui \
    uithreadadder.ui \
    dialogfoldershortcut.ui

RESOURCES += \
    resources.qrc

win32: RC_FILE = fourchan-dl-win.rc
os2: RC_FILE = fourchan-dl-os2.rc
macx: ICON = resources/fourchan-dl.icns

BUILDDATE = __DATE__
DEFINES += BUILDDATE=$${BUILDDATE} PROGRAM_VERSION="\\\"1.3.experimental\\\""

OTHER_FILES += \
    readme.txt \
    settings.ini \
    fourchan-dl.rc








