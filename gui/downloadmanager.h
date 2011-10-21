#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

//#include <QThread>
#include <QObject>
#include <QUrl>
#include <QtNetwork>
#include <QSettings>
#include "requesthandler.h"
#include "supervisednetworkreply.h"
#include "downloadrequest.h"
#include "networkaccessmanager.h"

class RequestHandler;
class DownloadRequest;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    QByteArray getByteArray(qint64);
    void freeRequest(qint64);
    void removeRequest(qint64);
    qint64 requestDownload(RequestHandler* caller, QUrl url, int prio=0);
    void getStatistics(int* files, float* kbytes);
    int getPendingRequests();
    int getRunningRequests();

private:
    QList<NetworkAccessManager*> nams;
    QHash<qint64, DownloadRequest*> requestList;
    QHash<qint64, QNetworkReply*> activeReplies;
    QHash<qint64, SupervisedNetworkReply*> supervisors;
    QMultiMap<int, qint64> priorities;
    QSettings* settings;
    QTimer* waitTimer;
    NetworkAccessManager* getFreeNAM();
    void setupNetworkAccessManagers(int count);
    int maxRequests;
    int currentRequests;
    int totalRequests;
    int finishedRequests;
    int initialTimeout;
    int runningTimeout;
    bool serviceAvailable;
    int statistic_downloadedFiles;
    float statistic_downloadedKBytes;

    void addRequest(qint64, DownloadRequest*);
    void handleError(qint64, QNetworkReply*);
    void reschedule(qint64);
    void processRequests();
    inline qint64 getUID() {return ++lastid;}
    qint64 lastid;


private slots:
    void replyFinished(QNetworkReply*);
    void startRequest(qint64);
//    void dlProgress(qint64 b, qint64 t) {/*qDebug() << QString("%1 of %2").arg(b).arg(t);*/}
    void replyError(QNetworkReply::NetworkError e) {qDebug() << "reply error " << e;}
    void downloadTimeout(qint64);
    void resumeDownloads();

signals:
    void totalRequestsChanged(int);
    void finishedRequestsChanged(int);
    void error(QString);
public slots:
    void loadSettings();
};
#endif // DOWNLOADMANAGER_H