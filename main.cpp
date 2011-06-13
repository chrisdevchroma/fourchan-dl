#include <QtGui/QApplication>
#include <QtDebug>
#include <QCleanlooksStyle>
#include "mainwindow.h"
#include "thumbnailremoverthread.h"

#if QT_VERSION < 0x040000
 #error "Sorry mate, this application needs Qt4.x.x to run properly."
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("plastique");
    MainWindow w;
    w.show();

    ThumbnailRemoverThread t;
    t.start(QThread::LowPriority);

    return a.exec();
}
