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
    uilisteditor.cpp

HEADERS  += mainwindow.h \
    parser.h \
    ui4chan.h \
    uiinfo.h \
    uiconfig.h \
    thumbnailthread.h \
    uilisteditor.h

FORMS    += mainwindow.ui \
    ui4chan.ui \
    uiinfo.ui \
    uiconfig.ui \
    uilisteditor.ui

RESOURCES += \
    resources.qrc

BUILDDATE = __DATE__
DEFINES += BUILDDATE=$${BUILDDATE} PROGRAM_VERSION="\\\"0.9.5\\\""

OTHER_FILES += \
    readme.txt \
    settings.ini
