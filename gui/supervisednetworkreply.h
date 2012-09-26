#ifndef SUPERVISEDNETWORKREPLY_H
#define SUPERVISEDNETWORKREPLY_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QtDebug>
#include <QTime>
#include "QsLog.h"

class SupervisedNetworkReply : public QObject
{
    Q_OBJECT
public:
    explicit SupervisedNetworkReply(QObject *parent = 0);
    void setNetworkReply(QNetworkReply*, qint64 uid);
    void removeNetworkReply();
    QNetworkReply* getReply()   {return reply;}
    QString getUrl()            {return url;}
    bool hasAborted()           {return aborted;}
    void clearAbortedFlag()     {aborted = false;}
    void setTimeouts(int initial, int running);
private:
    QTimer* timer;
    qint64 received;
    qint64 total;
    qint64 _uid;
    QNetworkReply* reply;
    bool aborted;
    QString url;
    QTime time;
    int _initialTimeout;
    int _runningTimeout;
signals:
    void timeout(qint64);
private slots:
    void monitorDownload(qint64 received, qint64 total);
    void handleTimeout();
public slots:

};

#endif // SUPERVISEDNETWORKREPLY_H
