#include <QtGui/QApplication>
#include <QtDebug>
#include <QCleanlooksStyle>
#include "mainwindow.h"
#include "downloadmanager.h"
#include "thumbnailthread.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif

DownloadManager* downloadManager;
ThumbnailThread* tnt;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("plastique");
    downloadManager = new DownloadManager();
    tnt = new ThumbnailThread();
    tnt->start(QThread::NormalPriority);

    MainWindow w;
    w.show();

    w.restoreTabs();

    return a.exec();
}
