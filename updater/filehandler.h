#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "types.h"

class FileHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = 0);
    QList<FileUpdate> getFailedFiles() {return failedFiles;}

private:
    QList<FileUpdate> updateList;
    QList<FileUpdate> failedFiles;
    QTextStream* output;

    void p(QString);

signals:
    void exchangingFinished(bool);
    void error(QString);

public slots:
    void startExchange(QList<FileUpdate>);

};

#endif // FILEHANDLER_H
