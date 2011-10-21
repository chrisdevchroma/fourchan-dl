#include "networkaccessmanager.h"

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    m_activeRequests = 0;
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished()));
}

QNetworkReply* NetworkAccessManager::get(const QNetworkRequest &request) {
    m_activeRequests++;

    return QNetworkAccessManager::get(request);
}

void NetworkAccessManager::requestFinished() {
    if (--m_activeRequests < 0)
        m_activeRequests = 0;
}
