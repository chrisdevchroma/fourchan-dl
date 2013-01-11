#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include "defines.h"
#include "downloadmanager.h"

class DownloadManager;
extern DownloadManager* downloadManager;

class RequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit RequestHandler(QObject *parent = 0);
    void request(QUrl, int priority=-1);
    void cancel(QUrl url);
    void cancelAll();
    void requestFinished(qint64);
    void error(qint64, int);
private:
    QHash<qint64,QUrl> requests;
signals:
    void response(QUrl, QByteArray);
    void responseError(QUrl, int);
public slots:
};

#endif // REQUESTHANDLER_H
