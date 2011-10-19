#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = ../../parser4chan
else:TARGET = ../parser4chan
TEMPLATE = lib
CONFIG += dll plugin

DEFINES += FOURCHANPARSER_LIBRARY _LIB_VERSION="\\\"1.0\\\""

SOURCES += parser4chan.cpp

HEADERS += parser4chan.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3114540
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = parser4chan.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
