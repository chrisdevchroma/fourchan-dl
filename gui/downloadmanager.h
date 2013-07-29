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
#include "QsLog.h"

class RequestHandler;
class DownloadRequest;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    QByteArray getByteArray(qint64);
    bool cached(qint64);
    void freeRequest(qint64);
    void removeRequest(qint64);
    qint64 requestDownload(RequestHandler* caller, QUrl url, int prio=0);
    void getStatistics(int* files, float* kbytes);
    int getPendingRequests();
    int getRunningRequests();
    int getTotalRequests();
    int getFinishedRequests();
    void pauseDownloads();
    void setMaxPriority(int);
    QMap<qint64, QString> getPendingRequestsMap();
    QMap<qint64, QString> getRunningRequestsMap();
    QString getFilenameForURL(QUrl);

private:
    QList<NetworkAccessManager*> nams;
    QHash<qint64, DownloadRequest*> requestList;
    QHash<qint64, QNetworkReply*> activeReplies;
    QHash<qint64, SupervisedNetworkReply*> supervisors;
    QMultiMap<int, qint64> priorities;
    QSettings* settings;
    QTimer* waitTimer;
    NetworkAccessManager* getFreeNAM();
//    QNetworkAccessManager* _manager;
    void setupNetworkAccessManagers(int count);
    int maxRequests;
    int currentRequests;
    int totalRequests;
    int finishedRequests;
    int initialTimeout;
    int runningTimeout;
    bool downloadsPaused;
    int statistic_downloadedFiles;
    float statistic_downloadedKBytes;
    bool _useThreadCache;
    QString _threadCachePath;

    void addRequest(qint64, DownloadRequest*);
    void handleError(qint64, QNetworkReply*);
    void reschedule(qint64);
    void processRequests();
    inline qint64 getUID() {return ++lastid;}

    bool cacheAvailable(QUrl url);
    QByteArray getCachedReply(QUrl url);
    QString encodeURL(QString);
    QString decodeURL(QString);

    qint64 lastid;
    int _max_priority;


private slots:
    void replyFinished(QNetworkReply*);
    void startRequest(qint64);
    void replyError(QNetworkReply::NetworkError e) {QLOG_ERROR() << "DownloadManager :: reply error " << e;}
    void downloadTimeout(qint64);

signals:
    void totalRequestsChanged(int);
    void finishedRequestsChanged(int);
    void error(QString);
public slots:
    void loadSettings();
    void resumeDownloads();
};
#endif // DOWNLOADMANAGER_H
