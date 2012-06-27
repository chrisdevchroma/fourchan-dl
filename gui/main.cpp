#include <QtGui/QApplication>
#include <QtDebug>
#include <QCleanlooksStyle>
#include <QFile>
#include "mainwindow.h"
#include "downloadmanager.h"
#include "thumbnailthread.h"
#include "foldershortcuts.h"
#include "pluginmanager.h"
#include "uiimageviewer.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif

DownloadManager* downloadManager;
ThumbnailThread* tnt;
FolderShortcuts* folderShortcuts;
MainWindow* mainWindow;
PluginManager* pluginManager;
QString updaterFileName;
UIImageViewer* imageViewer;

void checkEnvironment();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("plastique");

    checkEnvironment();

    downloadManager = new DownloadManager();
    downloadManager->pauseDownloads();  // Do not download anything until we are fully set

    tnt = new ThumbnailThread();
    tnt->start(QThread::NormalPriority);

    folderShortcuts = new FolderShortcuts();

    pluginManager = new PluginManager();

    mainWindow = new MainWindow();
    mainWindow->show();
    mainWindow->restoreTabs();

    imageViewer = new UIImageViewer(mainWindow);

    downloadManager->resumeDownloads();

    a.connect(&a, SIGNAL(aboutToQuit()), mainWindow, SLOT(saveSettings()));
    a.connect(&a, SIGNAL(aboutToQuit()), mainWindow, SLOT(removeTrayIcon()));

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
//        qDebug() << "updater directory does not exists. Creating " << QString("%1/updater").arg(dir.absolutePath());
        dir.mkdir("updater");
    }

    // Check if all files are present
    foreach (QString filename, neededFiles) {
        if (!QFile::exists(QString("%1/%2").arg(updaterDir.absolutePath()).arg(filename))) {
//            qDebug() << filename << "does not exists - copying from application dir";
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
        qDebug() << "No updater found!";
    }

    // Check for plugin folders
    if (!pluginDir.exists()) {
//        qDebug() << "plugin directory does not exists. Creating " << QString("%1/plugins").arg(dir.absolutePath());
        dir.mkdir("plugins");
    }
}
