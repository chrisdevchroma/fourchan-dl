#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QtNetwork>
#include <QSettings>
#include "updater.h"

QTextStream* output;
QTextStream* foutput;
QSettings* settings;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    output = new QTextStream(stdout);

    settings = new QSettings("../settings.ini", QSettings::IniFormat);

    QFile f;

    f.setFileName("update.log");
    f.open(QIODevice::Append);

    foutput = new QTextStream(&f);
    *foutput << "\n\n---------------\n";
    *foutput << "Started " << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << "\n";
    foutput->flush();

    Updater updater;
    updater.run();

    return a.exec();
}
