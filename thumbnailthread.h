#ifndef THUMBNAILTHREAD_H
#define THUMBNAILTHREAD_H

#include <QThread>
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
#include "uiimageoverview.h"

class UIImageOverview;

class ThumbnailThread : public QThread
{
    Q_OBJECT
public:
    explicit ThumbnailThread(QObject *parent = 0);
    void setIconSize(QSize s);
    void createThumbnails(void);
    void addToList(UIImageOverview* caller, QString s);
    bool cancelAll(UIImageOverview* caller);
    QString getCacheFile(QString);
private:
    QStringList list;
    QSize* iconSize;
    bool hq;
    volatile bool canceled;
    QSettings* settings;
    QMutex mutex;
    QWaitCondition condition;
    bool newImages;
    QString cacheFolder;
    bool useCache;
    QMultiMap<QString, UIImageOverview*> callingUIs;

protected:
    void run();
public slots:

signals:
    void thumbnailCreated(QString, QImage);
    void pendingThumbnails(int);
};

#endif // THUMBNAILTHREAD_H
