#ifndef THUMBNAILREMOVER_H
#define THUMBNAILREMOVER_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QString>
#include <QMutex>
#include <QCoreApplication>
#include <QFileInfoList>
#include <QtDebug>
#include <QDateTime>
#include "QsLog.h"

class ThumbnailRemover : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailRemover(QObject *parent = 0);
    
private:
    QSettings* settings;
    int ttl;
    QString dirName;
    QDir dir;
    QMutex mutex;
    QFileInfoList fileInfoList;

signals:
    
public slots:
    void removeFiles(QStringList fileList);
    void removeAll();
    void removeOutdated();
    
};

#endif // THUMBNAILREMOVER_H
