#ifndef DEFINES_H
#define DEFINES_H

#ifdef Q_OS_WIN32
#define APP_NAME        "fourchan-dl.exe"
#define UPDATER_NAME    "au.exe"
#define USE_UPDATER
#else
#ifdef Q_OS_LINUX
#define APP_NAME        "fourchan-dl"
#define UPDATER_NAME    "au"
#define USE_UPDATER
#endif
#endif

#endif // DEFINES_H
