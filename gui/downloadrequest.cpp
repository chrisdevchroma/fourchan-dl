#include "downloadrequest.h"

DownloadRequest::DownloadRequest(QObject *parent) :
    QObject(parent)
{
    pauseTimer = new QTimer(this);
    pauseTimer->setSingleShot(true);

    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(pauseTimerTriggered()));

    _paused = false;
    reset();
    _url = QUrl("");
    _prio = 0;
    _cached_reply = false;
    _error_count = 0;
}

void DownloadRequest::reset() {
    _finished = false;
    _processing = false;
    _error_count = 0;
}

void DownloadRequest::setResponse(QByteArray ba) {
     _response = ba;
     _finished = true;
}

void DownloadRequest::pause(int s) {
    if (s <= 0 ) {
        s = 10;
    }

    _paused = true;
    pauseTimer->setInterval(s*1000);
    pauseTimer->start();
}

void DownloadRequest::pauseTimerTriggered() {
    _paused = false;

    emit requestUnpaused();
}

void DownloadRequest::download_error(int increment) {
    _error_count += increment;
    pause(10);
}
