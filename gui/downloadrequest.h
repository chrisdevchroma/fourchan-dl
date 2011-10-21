#ifndef DOWNLOADREQUEST_H
#define DOWNLOADREQUEST_H

#include <QObject>
#include "requesthandler.h"

class RequestHandler;

class DownloadRequest : public QObject
{
    Q_OBJECT
public:
    explicit DownloadRequest(QObject *parent = 0);

    void setRequestHandler(RequestHandler* rh) {_rh = rh;}
    RequestHandler* requestHandler() {return _rh;}

    void setUrl(QUrl url) {_url = url;}
    QUrl url() {return _url;}

    void setPriority(int prio) {_prio = prio;}
    int priority() {return _prio;}

    QByteArray response(void) {return _response;}
    void setResponse(QByteArray ba);

    bool processing() {return _processing;}
    void setProcessing(bool b) {_processing = b;}
    void reset();
    bool finished() {return _finished;}

private:
    RequestHandler* _rh;
    QUrl _url;
    int _prio;
    QByteArray _response;
    bool _processing;
    bool _finished;

signals:

public slots:

};

#endif // DOWNLOADREQUEST_H
