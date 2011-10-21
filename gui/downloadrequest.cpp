#include "downloadrequest.h"

DownloadRequest::DownloadRequest(QObject *parent) :
    QObject(parent)
{
    reset();
    _url = QUrl("");
    _prio = 0;
}

void DownloadRequest::reset() {
    _finished = false;
    _processing = false;
}

void DownloadRequest::setResponse(QByteArray ba) {
     _response = ba;
     _finished = true;
}
