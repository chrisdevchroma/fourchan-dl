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

class ThumbnailThread : public QThread
{
    Q_OBJECT
public:
    explicit ThumbnailThread(QObject *parent = 0);
    void setIconSize(QSize s);
    void createThumbnails(void);
    void addToList(QString s);
private:
    QStringList list;
    QSize* iconSize;
    bool hq;
    QSettings* settings;
    QMutex mutex;
    QWaitCondition condition;
    bool newImages;

protected:
    void run();
public slots:

signals:
    void thumbnailCreated(QString, QImage);
};

#endif // THUMBNAILTHREAD_H
