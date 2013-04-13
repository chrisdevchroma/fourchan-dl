#include "httprequest.h"

HTTPRequest::HTTPRequest(QObject *parent) :
    QObject(parent)
{
    socket.bind();
}

void HTTPRequest::setUrl(QUrl u) {
    _url = u;
}

void HTTPRequest::start() {
    if (_url.isValid()) {
        socket.connectToHost(_url.host(), _url.port());
    }
}
