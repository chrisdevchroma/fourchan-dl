#include "supervisednetworkreply.h"

SupervisedNetworkReply::SupervisedNetworkReply(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    time = QTime();
    _initialTimeout = 30;
    _runningTimeout = 2;
    timer->setInterval(_initialTimeout);
    timer->setSingleShot(true);

    received = 0;
    total = 0;
    aborted = false;

    reply = 0;

//    connect(timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
}

void SupervisedNetworkReply::setNetworkReply(QNetworkReply *r, qint64 uid) {
    if (reply != 0)
        removeNetworkReply();

    reply = r;
    _uid = uid;
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(monitorDownload(qint64,qint64)));
    connect(reply, SIGNAL(finished()), timer, SLOT(stop()));

    time.start();

    timer->start();
    QLOG_TRACE() << "SupervisedNetworkReply :: " << QTime::currentTime() << "Starting timer for " << reply->url().toString();
//    this->setParent(reply);
    url = reply->url().toString();
}

void SupervisedNetworkReply::removeNetworkReply() {
    timer->stop();
    disconnect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(monitorDownload(qint64,qint64)));
    disconnect(reply, SIGNAL(finished()), timer, SLOT(stop()));
    this->setParent(0);
    reply = 0;
    aborted = false;
    url = "";
}

void SupervisedNetworkReply::monitorDownload(qint64 rec, qint64 tot) {
//    QLOG_TRACE() << "SupervisedNetworkReply :: " << reply->url().toString() << QString("loaded %1/%2 bytes / internal values %3/%4").arg(rec).arg(tot).arg(received).arg(total);
    if (total == 0) {
        timer->stop();
        timer->setInterval(_runningTimeout);
        total = tot;
        received = rec;
        timer->start();
    }
    else {
        if (rec == tot) {
            timer->stop();
            timer->setInterval(_runningTimeout);
            tot = 0;
        }
        else if (rec > received) {
            timer->start();
            received = rec;
        }
    }
}

void SupervisedNetworkReply::handleTimeout() {
    QLOG_INFO() << "SupervisedNetworkReply :: " << QTime::currentTime().toString("hh:mm:ss") << _uid << "timed out after" << QString("%1 seconds").arg(time.elapsed()/1000) << "; received" << received << "of" << total << "bytes";

    emit timeout(_uid);
}

void SupervisedNetworkReply::setTimeouts(int initial, int running) {
    _initialTimeout = initial;
    _runningTimeout = running;
    if (!timer->isActive())
        timer->setInterval(_initialTimeout);
}
