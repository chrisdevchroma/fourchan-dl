#ifndef THUMBNAILCREATOR_H
#define THUMBNAILCREATOR_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QStringList>
#include <QTimer>
#include <QSettings>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include "QsLog.h"

class ThumbnailCreator : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailCreator(QObject *parent = 0);
    void setIconSize(QSize s);
    void addToList(QString s);
    QString getCacheFile(QString);
    void halt();
    void resume();

private:
    QStringList list;
    QSize* iconSize;
    bool hq;
    volatile bool canceled;
    volatile bool halted;
    QSettings* settings;
    bool newImages;
    QString cacheFolder;
    bool useCache;
    QMutex mutex;
    QWaitCondition condition;
    
signals:
    void pendingThumbnails(int);
    void thumbnailAvailable(QString, QString);
    
public slots:
    void go();
    
};

#endif // THUMBNAILCREATOR_H
