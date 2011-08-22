#include <QtGui/QApplication>
#include <QtDebug>
#include <QCleanlooksStyle>
#include "mainwindow.h"
#include "downloadmanager.h"
#include "thumbnailthread.h"
#include "foldershortcuts.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif

DownloadManager* downloadManager;
ThumbnailThread* tnt;
FolderShortcuts* folderShortcuts;
MainWindow* mainWindow;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("plastique");
    downloadManager = new DownloadManager();
    tnt = new ThumbnailThread();
    tnt->start(QThread::NormalPriority);

    folderShortcuts = new FolderShortcuts();

    mainWindow = new MainWindow();
    mainWindow->show();
    mainWindow->restoreTabs();

    a.connect(&a, SIGNAL(lastWindowClosed()), mainWindow, SLOT(saveSettings()));
//    MainWindow w;
//    w.show();

//    w.restoreTabs();

    return a.exec();
}
