#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QtNetwork>
#include <QRegExp>
#include "types.h"

extern QTextStream* output;
extern QTextStream* foutput;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);

private:
    QUrl uri;
    QString sURI;
    QString savePath;
    QString html;
    QNetworkAccessManager* manager;
    QNetworkCookieJar* cookies;
    QList<FileUpdate> updateList;

    void p(QString);
    void initiateDownload();

private slots:
    void replyFinished(QNetworkReply*);
    void replyError(QNetworkReply::NetworkError e);

signals:
    void downloadsFinished(QList<FileUpdate>);
    void error(QString);

public slots:
    void startDownload(QList<FileUpdate>);
};

#endif // DOWNLOADMANAGER_H
