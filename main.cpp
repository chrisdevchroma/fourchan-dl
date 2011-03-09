#include <QtGui/QApplication>
#include <QtDebug>
#include <QCleanlooksStyle>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCleanlooksStyle s();

//    a.setStyle(&s);
    a.setStyle("plastique");
//    a.setStyle("cleanlooks");
    MainWindow w;
    w.show();

    return a.exec();
}
