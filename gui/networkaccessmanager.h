#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = 0);
    QNetworkReply* get ( const QNetworkRequest &request );
    int activeRequests() {return m_activeRequests;}
private:
    int m_activeRequests;

private slots:
    void requestFinished(void);
signals:

public slots:

};

#endif // NETWORKACCESSMANAGER_H
