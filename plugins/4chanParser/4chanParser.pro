#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T19:19:51
#
#-------------------------------------------------

QT       -= gui

TARGET = ../../4chanParser
TEMPLATE = lib

DEFINES += FOURCHANPARSER_LIBRARY _LIB_VERSION="\\\"1.0\\\""

SOURCES += parser4chan.cpp

HEADERS += parser4chan.h\
        4chanParser_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3114540
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = 4chanParser.dll
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
