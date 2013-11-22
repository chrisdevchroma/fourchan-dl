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
    QString addToList(QString s);
    QString getCacheFile(QString);
    void halt();
    void resume();
    void wakeup();

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

    QTimer* event_emit_timer;
    bool thumbnails_created;
    QStringList rendered_thumbnails;
    bool use_combined_thumbnail_list;

private slots:
    void eventEmitTimerTriggered();
    
signals:
    void pendingThumbnails(int);
    void thumbnailAvailable(QString, QString);
    void thumbnailsAvailable(QString);
    
public slots:
    void go();
    
};

#endif // THUMBNAILCREATOR_H
