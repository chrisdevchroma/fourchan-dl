#include <QCoreApplication>
#include <QtDebug>
#include <QFile>
#include <QDateTime>
#include <QThread>
#include "downloadmanager.h"
#include "pluginmanager.h"
#include "QsLog.h"
#include "QsLogDest.h"
#include "threadhandler.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif

DownloadManager* downloadManager;
PluginManager* pluginManager;
QString updaterFileName;
QFile* fLogFile;
QTextStream logOutput;
QTextStream in(stdin);
QTextStream out(stdout);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings settings("settings.ini", QSettings::IniFormat);
    int logLevel;

    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("fourchan-dl-console.log"));
    QsLogging::DestinationPtr fileDestination(
       QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
       QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());
#ifdef __DEBUG__
    logger.setLoggingLevel(QsLogging::TraceLevel);
#else
    logger.setLoggingLevel(QsLogging::WarnLevel);
#endif

    settings.setValue("console/version", PROGRAM_VERSION);
    settings.sync();

    logLevel = settings.value("options/log_level", -1).toInt();
    if (logLevel != -1) {
        logger.setLoggingLevel((QsLogging::Level)logLevel);
        QLOG_ALWAYS() << "APP :: Setting logging level to " << logLevel;
    }

    QLOG_ALWAYS() << "APP :: Console started";
    QLOG_ALWAYS() << "APP :: Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

    downloadManager = new DownloadManager();
    pluginManager = new PluginManager();

    downloadManager->pauseDownloads();  // Do not download anything until we are fully set
    ThreadHandler threadHandler;
    threadHandler.restoreThreads();

    a.connect(&a, SIGNAL(aboutToQuit()), &threadHandler, SLOT(saveSettings()));
//    a.connect(&a, SIGNAL(aboutToQuit()), &threadHandler, SLOT(deleteLater()));
//    a.connect(&a, SIGNAL(aboutToQuit()), downloadManager, SLOT(deleteLater()));

    downloadManager->resumeDownloads();

    return a.exec();
}
