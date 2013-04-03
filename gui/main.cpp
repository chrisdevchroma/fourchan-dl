#include <QApplication>
#include <QtDebug>

#if QT_VERSION < 0x050000
#include <QCleanlooksStyle>
#endif

#include <QFile>
#include <QDateTime>
#include <QThread>
#include "mainwindow.h"
#include "downloadmanager.h"
//#include "thumbnailthread.h"
#include "thumbnailcreator.h"
#include "foldershortcuts.h"
#include "pluginmanager.h"
#include "uiimageviewer.h"
#include "QsLog.h"
#include "QsLogDest.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif

DownloadManager* downloadManager;
ThumbnailCreator* tnt;
FolderShortcuts* folderShortcuts;
PluginManager* pluginManager;
QString updaterFileName;
UIImageViewer* imageViewer;
QFile* fLogFile;
QTextStream logOutput;
MainWindow* mainWindow;

void checkEnvironment();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("settings.ini", QSettings::IniFormat);
    int logLevel;

#if QT_VERSION >= 0x050000
    a.setStyle("fusion");
#else
    a.setStyle("cleanlooks");
#endif

    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("fourchan-dl.log"));
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

    logLevel = settings.value("options/log_level", -1).toInt();
    if (logLevel != -1) {
        logger.setLoggingLevel((QsLogging::Level)logLevel);
        QLOG_ALWAYS() << "APP :: Setting logging level to " << logLevel;
    }

    QLOG_INFO() << "APP :: Program started";
    QLOG_INFO() << "APP :: Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

    checkEnvironment();
    downloadManager = new DownloadManager();
    folderShortcuts = new FolderShortcuts();
    pluginManager = new PluginManager();

    downloadManager->pauseDownloads();  // Do not download anything until we are fully set

    QThread thumbnailThread;
    thumbnailThread.setPriority(QThread::NormalPriority);
    tnt = new ThumbnailCreator();
    tnt->moveToThread(&thumbnailThread);

    a.connect(&thumbnailThread, SIGNAL(started()), tnt, SLOT(go()));
    downloadManager->resumeDownloads();


    thumbnailThread.start();

    mainWindow = new MainWindow();
    imageViewer = new UIImageViewer(mainWindow);

    mainWindow->show();
    mainWindow->restoreTabs();

    a.connect(&a, SIGNAL(aboutToQuit()), mainWindow, SLOT(aboutToQuit()));
    a.connect(&a, SIGNAL(aboutToQuit()), &thumbnailThread, SLOT(terminate()));

    return a.exec();
}

void checkEnvironment() {
    QDir dir;
    QDir updaterDir;
    QDir pluginDir;
    QStringList neededFiles;
    QFile f;

    dir.setPath(QApplication::applicationDirPath());
    updaterDir.setPath(dir.path()+"/updater");
    pluginDir.setPath(dir.path()+"/plugins");

#ifdef Q_OS_WIN32
    neededFiles << "QtCore4.dll" << "QtNetwork4.dll" << "mingwm10.dll" << "libgcc_s_dw2-1.dll" << "au.exe" << "upd4t3r.exe";
#endif

    // Check for updater folders
    if (!updaterDir.exists()) {
        QLOG_INFO() << "APP :: updater directory does not exists. Creating " << QString("%1/updater").arg(dir.absolutePath());
        dir.mkdir("updater");
    }

    // Check if all files are present
    foreach (QString filename, neededFiles) {
        if (!QFile::exists(QString("%1/%2").arg(updaterDir.absolutePath()).arg(filename))) {
            QLOG_INFO() << "APP :: " << filename << "does not exists - copying from application dir";
            f.copy(QString("%1/%2").arg(dir.absolutePath()).arg(filename), QString("%1/%2").arg(updaterDir.absolutePath()).arg(filename));
        }
    }

    updaterFileName = "";
    if (QFile::exists("updater/upd4t3r.exe")) {
        // The new updater is already present
        // Use this for updating the components
        // If old updater still exists, delete it because it's not needed anymore
        updaterFileName = "updater/upd4t3r.exe";
        QFile::remove("au.exe");
        QFile::remove("updater/au.exe");
    }
    else if (QFile::exists("updater/au.exe")) {
        updaterFileName = "updater/au.exe";
    }
    else {
        QLOG_WARN() << "APP :: No updater found!";
    }

    // Check for plugin folders
    if (!pluginDir.exists()) {
        QLOG_INFO()  << "APP :: Plugin directory does not exists. Creating " << QString("%1/plugins").arg(dir.absolutePath());
        dir.mkdir("plugins");
    }
}
