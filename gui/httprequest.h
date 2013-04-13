#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QUrl>

class HTTPRequest : public QObject
{
    Q_OBJECT
public:
    explicit HTTPRequest(QObject *parent = 0);
    QByteArray readAll();
    QUrl url() {return _url;}
    void setUrl(QUrl u);
    void start();
    void abort();

signals:
    void finished();
    void error();
    void aborted();

public slots:
    

private:
    QUrl _url;
    QTcpSocket socket;
};

#endif // HTTPREQUEST_H
