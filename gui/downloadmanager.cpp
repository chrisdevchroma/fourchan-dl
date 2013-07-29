#include "downloadmanager.h"

#define MAX_CONCURRENT_DOWNLOADS_PER_NAM 5

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    downloadsPaused = true;
    setMaxPriority(0);
    nams.clear();
    nams.append(new NetworkAccessManager(this));    // Add at least one AccessManager
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    waitTimer = new QTimer();
    waitTimer->setSingleShot(true);
    waitTimer->setInterval(20000);

    requestList.clear();
    lastid = 0;
    currentRequests = 0;

    totalRequests = 0;
    finishedRequests = 0;

    statistic_downloadedFiles  = 0;
    statistic_downloadedKBytes = 0;

    loadSettings();

//    _manager = new QNetworkAccessManager(this);
//    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    connect(nams.at(0), SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(waitTimer, SIGNAL(timeout()), this, SLOT(resumeDownloads()));
}

void DownloadManager::loadSettings() {
    settings->beginGroup("download_manager");
        maxRequests = settings->value("concurrent_downloads", 20).toInt();
        initialTimeout = settings->value("initial_timeout", 30).toInt()*1000;
        runningTimeout = settings->value("running_timeout", 20).toInt()*1000;
        _useThreadCache = settings->value("use_thread_cache", false).toBool();
        _threadCachePath = settings->value("thread_cache_path", "").toString();
    settings->endGroup();

    settings->beginGroup("statistics");
        statistic_downloadedFiles = settings->value("downloaded_files", 0).toFloat();
        statistic_downloadedKBytes = settings->value("downloaded_kbytes", 0).toFloat();
    settings->endGroup();

    setupNetworkAccessManagers(qCeil(maxRequests/MAX_CONCURRENT_DOWNLOADS_PER_NAM));
}

void DownloadManager::replyFinished(QNetworkReply* reply) {
    QString redirect;
    qint64 uid;
    QList<QNetworkReply*> replies;
    DownloadRequest* dr;
    QList<QByteArray> bal;
    QFile f;
    QString threadCacheFilename;


    // Search in requestList for this reply
    bal = reply->rawHeaderList();
    QLOG_TRACE() << "DownloadManager :: " << reply->url().toString() << "rawHeader: " << bal;
    replies = activeReplies.values();
    uid = activeReplies.key(reply, -1);
    dr = requestList.value(uid,0);
    //supervisors.value(uid)->deleteLater();
    supervisors.remove(uid);
    priorities.remove(priorities.key(uid), uid);
    activeReplies.remove(uid);

    QLOG_TRACE() << "DownloadManager :: " << "Finished request" << uid << reply->url().toString() << "reply" << (qint64)reply;

    if (uid == 0) {
        QLOG_INFO() << "DownloadManager :: " << "uid 0 finished; url=" << reply->url().toString();
    }
    if (uid != -1) {
        if (reply->bytesAvailable() < reply->header(QNetworkRequest::ContentLengthHeader).toLongLong()) {
            QLOG_WARN() << "DownloadManager :: " << "Received less byte than expected - Possibly because the download timed out";
            reschedule(uid);
        }
        else {
            redirect = reply->header(QNetworkRequest::LocationHeader).toString();
            if (!redirect.isEmpty()) {
                if (dr != 0) {
                    dr->setUrl(QUrl(redirect));
                    dr->setProcessing(false);
                    priorities.insertMulti(-1, uid);
                }
            }
            else if (reply->error() != QNetworkReply::NoError) {
                handleError(uid, reply);
            }
            else {
                if (reply->isFinished()) {
                    dr->setResponse(reply->readAll());
                    dr->requestHandler()->requestFinished(uid);

                    emit finishedRequestsChanged(++finishedRequests);
                    if (reply->url().toString().indexOf(QRegExp(__IMAGE_REGEXP__, Qt::CaseInsensitive)) != -1) {
                        statistic_downloadedFiles++;
                        statistic_downloadedKBytes += ((reply->header(QNetworkRequest::ContentLengthHeader).toFloat())/1024);
                    }
                    else {
                        if (_useThreadCache) {
                            threadCacheFilename = getFilenameForURL(reply->url());
                            if (threadCacheFilename != ".") {
                                f.setFileName(threadCacheFilename);
                                f.open(QIODevice::WriteOnly | QIODevice::Truncate);
                                if (f.isOpen() && f.isWritable()) {
                                    f.write(qCompress(dr->response()));
                                    QLOG_DEBUG() << "DownloadManager :: Writing cache file " << threadCacheFilename;
                                }
                                f.close();
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        QLOG_ERROR() << "DownloadManager :: " << "I did not find the request to the reply!";
    }

    currentRequests--;
    reply->deleteLater();

    if (activeReplies.count() > (maxRequests+10)) {
        QLOG_WARN() << "DownloadManager :: " << "There are more active requests than allowed. Pausing downloads.";
        pauseDownloads();

        foreach (QNetworkReply* r, activeReplies) {
            r->abort();
        }

        waitTimer->start();
    }

    processRequests();
}

QByteArray DownloadManager::getByteArray(qint64 uid) {
    QByteArray ret;
    DownloadRequest* dr;

    dr = requestList.value(uid,0);

    if (dr != 0) {
        ret = dr->response();
    }

    return ret;
}

bool DownloadManager::cached(qint64 uid) {
    DownloadRequest* dr;
    bool ret;

    dr = requestList.value(uid,0);

    ret = false;
    if (dr != 0) {
        ret = dr->cached();
    }

    return ret;
}

void DownloadManager::freeRequest(qint64 uid) {
    DownloadRequest* dr;
    dr = requestList.value(uid, 0);
    QLOG_TRACE() << "DownloadManager :: " << "freeRequest(" << uid << ")";

    if (dr != 0) {
        dr->deleteLater();
        requestList.remove(uid);
        activeReplies.remove(uid);
    }
//    QLOG_TRACE() << "DownloadManager :: " << "open requests" << requestList.keys();
//    QLOG_TRACE() << "DownloadManager :: " << "priorities" << priorities;
}

qint64 DownloadManager::requestDownload(RequestHandler* caller, QUrl url, int prio) {
    DownloadRequest* dr;
    qint64 uid;

    dr = new DownloadRequest();
    dr->setRequestHandler(caller);
    dr->setUrl(url);
    dr->setPriority(prio);

    connect(dr, SIGNAL(requestUnpaused()), this, SLOT(resumeDownloads()));

    uid = getUID();

//    addRequest(uid, dr);
    requestList.insert(uid, dr);
    priorities.insertMulti(prio, uid);

    processRequests();

    emit totalRequestsChanged(++totalRequests);

    return uid;
}

/**
 * Add Request to internal list and start Thread/Download immediately if max. number of downloads is not met.
 */
void DownloadManager::addRequest(qint64 uid, DownloadRequest* dr) {
    requestList.insert(uid, dr);

    processRequests();
    emit totalRequestsChanged(++totalRequests);
}

void DownloadManager::downloadTimeout(qint64 uid) {
    QNetworkReply* r;

    r = activeReplies.value(uid);

    if (r != 0) {
//        QLOG_TRACE() << "DownloadManager :: " << uid << QString(r->readAll());
        r->abort();
    }
}

void DownloadManager::processRequests() {
    QList<qint64> uids;

    if (!downloadsPaused) {
        uids = priorities.values();

        if (uids.count() > 0) {
            foreach (qint64 uid, uids) {
                if (activeReplies.count() >= maxRequests) {
                    break;
                }
                else {
                    if (requestList.count(uid) > 0) {
                        if (!(requestList.value(uid)->finished()) &&
                            !(requestList.value(uid)->processing()) &&
                            !(requestList.value(uid)->paused())) {
                            startRequest(uid);
                            //                            QLOG_TRACE() << "DownloadManager :: " << "addRequest (" << uid << ")";
                        }
                    }
                }
            }
        }
        else {
            if (requestList.count() > 0) {
                // This should not happen but if there are no (more) priorities set for some downloads, reset them
                uids = requestList.keys();
                foreach(qint64 uid, uids) {
                    priorities.insertMulti(uid, 0);
                }

                processRequests();
            }
            else {
                // We are finished
                totalRequests = 0;
                finishedRequests = 0;
                emit totalRequestsChanged(totalRequests);
            }
        }
    }
}

void DownloadManager::startRequest(qint64 uid) {
    QNetworkRequest req;
    QNetworkReply* rep;
    NetworkAccessManager* nam;
    DownloadRequest* dr;
    SupervisedNetworkReply* sup;

    dr = requestList.value(uid, 0);

    if (dr != 0 && !downloadsPaused) {
        dr->setProcessing(true);
        sup = new SupervisedNetworkReply();
        sup->setTimeouts(initialTimeout, runningTimeout);
        connect(sup, SIGNAL(timeout(qint64)), this, SLOT(downloadTimeout(qint64)));

        req = QNetworkRequest(dr->url());
        req.setAttribute(QNetworkRequest::CookieSaveControlAttribute, QNetworkRequest::Automatic);
        req.setRawHeader("User-Agent", "Wget/1.12");
//        req.setRawHeader("User-Agent", "Opera/9.80 (Windows NT 6.1; U; en) Presto/2.9.168 Version/11.50");
        currentRequests++;
        nam = getFreeNAM();
        rep = nam->get(req);
//        rep = _manager->get(req);

        sup->setNetworkReply(rep, uid);
        supervisors.insert(uid, sup);

//        QLOG_TRACE() << "DownloadManager :: " << "Requesting" << uid << ":" << dr->url();// << "with reply" << (qint64)rep;
        activeReplies.insert(uid, rep);
    }
    else {
        QLOG_WARN() << "DownloadManager :: " << "Requested start of uid which is non-existent";
    }
}

void DownloadManager::handleError(qint64 uid, QNetworkReply* r) {
    DownloadRequest* dr;
    dr = requestList.value(uid);

    QLOG_WARN() << "DownloadManager :: " << r->url().toString() << "received error" << r->error() << ":" << r->errorString();

    switch (r->error()) {
    case 203:       // Not found
        if (_useThreadCache && cacheAvailable(dr->url())) {
                dr->setResponse(getCachedReply(dr->url()));
                dr->setCached(true);
                dr->requestHandler()->requestFinished(uid);
        }
        else {
            dr->requestHandler()->error(uid, 404);
        }

        currentRequests--;
        processRequests();

        break;

    case 301:   // Service unavailable
        QLOG_INFO() << "DownloadManager :: " << "Service unavailable" << r->url().host();
        emit error(QString("%1: Service unavailable").arg(r->url().host()));
        // Pause downloading to let the server relax
//        pauseDownloads();
        // Abort all downloads
//        foreach (QNetworkReply* r, activeReplies) {
//            r->abort();
//        }

//        waitTimer->start();
        dr->pause(10);
        reschedule(uid);
        break;
    case 205:
    case 99:
    case 299:
    case 5:         // aborted
    case 2:         // Connection closed
        //        QLOG_TRACE() << "DownloadManager :: " << "response:" << QString(r->readAll());
        //        QLOG_TRACE() << "DownloadManager :: " << "finished:" << r->isFinished();
        dr->pause(10);
        reschedule(uid);

//        emit message(QString("Rescheduled %1").arg(r->url().toString()));
        break;

    case 202:
        dr->requestHandler()->error(uid, 202);

        currentRequests--;
        processRequests();

    case 3:
        QLOG_WARN() << "DownloadManager :: " << "Host not found error for URL" << r->url().toString();
        currentRequests--;
        dr->pause(10);
        reschedule(uid);    // Try harder
        processRequests();
        break;

    default:
        QLOG_ERROR() << "DownloadManager :: " << "Unhandled error " << r->error();
//        r.caller->error(r.uid, 404);
        dr->pause(10);
        reschedule(uid);        // Since we don't know what happened, try harder
        currentRequests--;
        processRequests();

        break;
    }
}

void DownloadManager::reschedule(qint64 uid) {
    DownloadRequest* dr;
    int prio;

    QLOG_INFO() << "DownloadManager :: " << "rescheduling" << uid;
    dr = requestList.value(uid,0);
    if (dr != 0) {
        prio = dr->priority();
        dr->setProcessing(false);

        // "decrease priority for rescheduled downloads"
        priorities.remove(prio, uid);
        prio+=10;
        QLOG_INFO() << "DownloadManager :: " << uid << ":" << "setting new priority" << prio;
        dr->setPriority(prio);
        priorities.insertMulti(prio,uid);

        dr->reset();
        currentRequests--;
        processRequests();
    }
}

void DownloadManager::resumeDownloads() {
    QLOG_INFO() << "DownloadManager :: " << "resuming downloads";
    downloadsPaused = false;

    processRequests();
}

void DownloadManager::removeRequest(qint64 uid) {
    int prio;

    if (requestList.count(uid)>0) {
        prio = requestList.value(uid)->priority();
        priorities.remove(prio, uid);
        requestList.remove(uid);

        // See if this id is currently downloading
        if (activeReplies.count(uid) > 0) {
            supervisors.remove(uid);
            activeReplies.value(uid)->abort();
        }
    }
}

void DownloadManager::getStatistics(int *files, float *kbytes) {
    *files = statistic_downloadedFiles;
    *kbytes = statistic_downloadedKBytes;
}

NetworkAccessManager* DownloadManager::getFreeNAM() {
    NetworkAccessManager* ret;
    QMap<int, NetworkAccessManager*> load;

    for (int i=0; i<nams.count(); i++) {
        load.insertMulti(nams.at(i)->activeRequests(), nams.at(i));
    }

    ret = load.begin().value(); // Returns NetworkAccessManager with the least active requests

    return ret;
}

void DownloadManager::setupNetworkAccessManagers(int count) {
    for (int i=1; i<=count; i++) {
        nams.append(new NetworkAccessManager(this));
        connect(nams.last(), SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    }
}

int DownloadManager::getPendingRequests() {
    return priorities.count();
}

int DownloadManager::getRunningRequests() {
    return activeReplies.count();
}

int DownloadManager::getTotalRequests() {
    return totalRequests;
}

int DownloadManager::getFinishedRequests() {
    return finishedRequests;
}

void DownloadManager::pauseDownloads() {
    downloadsPaused = true;
}

void DownloadManager::setMaxPriority(int mp) {
    _max_priority = mp;

    QLOG_INFO() << "DownloadManager :: Setting max priority to " << mp;
    if (!downloadsPaused) {
        processRequests();
    }
}

QMap<qint64, QString> DownloadManager::getPendingRequestsMap() {
    QMap<qint64, QString> ret;
    QHashIterator<qint64, DownloadRequest*>  requests(requestList);

    while (requests.hasNext()) {
        requests.next();

        ret.insertMulti(requests.value()->priority(), requests.value()->url().toString());
    }

    return ret;
}

QMap<qint64, QString> DownloadManager::getRunningRequestsMap() {
    QMap<qint64, QString> ret;
    QHashIterator<qint64, QNetworkReply*>  requests(activeReplies);
/*
    while (requests.hasNext()) {
        requests.next();

        ret.insertMulti(0, requests.value()->url().toString());
    }
*/
    return ret;
}

bool DownloadManager::cacheAvailable(QUrl url) {
    bool ret;
    QFile f;

    ret = false;

    if (_useThreadCache && !_threadCachePath.isEmpty()) {
        if (QFile::exists(getFilenameForURL(url))) {
            ret = true;
        }
    }

    return ret;
}

QByteArray DownloadManager::getCachedReply(QUrl url) {
    QByteArray ret;
    QFile f;

    if (cacheAvailable(url)) {
        f.setFileName(getFilenameForURL(url));
        f.open(QIODevice::ReadOnly);

        if (f.isOpen() && f.isReadable()) {
            ret = qUncompress(f.readAll());
            QLOG_DEBUG() << "DownloadManager :: reading cache file for " << url.toString();
        }
        f.close();
    }

    return ret;
}

QString DownloadManager::encodeURL(QString in) {
    QString ret;

    ret = QUrl::toPercentEncoding(in);
    QLOG_DEBUG() << "DownloadManager :: encoding URL from " << in << " to " << ret;

    return ret;
}

QString DownloadManager::decodeURL(QString in) {
    QString ret;

    ret = QUrl::fromPercentEncoding(in.toUtf8());
    QLOG_DEBUG() << "DownloadManager :: decoding URL from " << in << " to " << ret;

    return ret;
}

QString DownloadManager::getFilenameForURL(QUrl url) {
    QString ret;

    if (!_threadCachePath.isEmpty()) {
        ret = QString("%1/%2.tcache").arg(_threadCachePath, encodeURL(url.toString()));
    }
    else {
        ret = ".";
    }

    QLOG_DEBUG() << "DownloadManager :: threadCache filename=" << ret;
    return ret;
}

