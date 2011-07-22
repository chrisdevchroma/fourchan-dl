#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QtDebug>
#include <QUrl>
#include <QtNetwork>
#include <QThread>
#include "blacklist.h"
#include "defines.h"
#include "requesthandler.h"
#include "supervisednetworkreply.h"

struct _IMAGE
{
    QString largeURI;
    QString thumbURI;
    QString originalFilename;
    QString savedAs;
    bool downloaded;
    bool requested;
};

//class Parser : public QThread
class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);
    int getTotalCount(void);
    int getDownloadedCount(void);
    bool getUrlOfFilename(QString, QString*);
    QStringList getDownloadedFiles();
    bool isFinished();
protected:
//    void run();

private:
    QUrl uri;
    QString sURI;
    QString savePath;
    QString html;
    QString values;
    QStringList rescheduled;
    QNetworkAccessManager* manager;
    QList<_IMAGE> images2dl;
    QTimer* rescheduleTimer;
    BlackList* blackList;
    RequestHandler* requestHandler;
    QList<SupervisedNetworkReply*> supervisedReplies;

    void parseHTML(void);
    bool addImage(_IMAGE img);
    int getNextImage(QString* s);
    bool blacklisted(QString s) {return blackList->contains(s);}
    void createSupervisedDownload(QUrl);
    void removeSupervisedDownload(QUrl);

    bool downloading;
    bool useOriginalFilename;
    int activeDownloads;
    int maxDownloads;

private slots:
//    void replyFinished(QNetworkReply*);
    void download(bool b);
    int setCompleted(QString, QString);
    void dlProgress(qint64 b, qint64 t) {qDebug() << QString("%1 of %2").arg(b).arg(t);}
//    void replyError(QNetworkReply::NetworkError e) {/*qDebug() << "reply error " << e;*/}
    void processSchedule();
    void reschedule(QString);
    void downloadAborted(QString);
    void processRequestResponse(QUrl, QByteArray);
//    void handleError(QNetworkReply*);
    void handleError(QUrl, int);

public slots:
    void setURI(QString pURI) { uri = QUrl(pURI); sURI = pURI;}
    void setSavePath(QString pSavePath) { savePath = pSavePath; }
    void startDownload(void);
    void stopDownload(void);
    void setUseOriginalFilename(int);
    void setValues(QString s) {values = s;}
    void setBlackList(BlackList* bl) {blackList = bl;}
    void setTimerInterval(int msec);
    void reloadFile(QString filename);
    void setMaxDownloads(int);

signals:
    void downloadFinished(void);
    void downloadsAvailable(bool);
    void totalCountChanged(int);
    void downloadedCountChanged(int);
    void fileFinished(QString);
    void error(int);
    void threadTitleChanged(QString);
    void tabTitleChanged(QString);
    void message(QString);
    void createTabRequest(QString);
    void closeTabRequest();
};

#endif // PARSER_H
