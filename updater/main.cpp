#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include "updater.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Updater updater;
    updater.run();

    return a.exec();
}
