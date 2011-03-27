#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QtDebug>
#include <QUrl>
#include <QtNetwork>

struct _IMAGE
{
    QString largeURI;
    QString thumbURI;
    QString originalFilename;
    bool downloaded;
    bool requested;
};

class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);

    void reloadFile(QString filename);
    void setMaxDownloads(int);
    int getTotalCount(void);
    int getDownloadedCount(void);
signals:

private:
    QUrl uri;
    QString sURI;
    QString savePath;
    QString html;
    QString values;
    QNetworkAccessManager* manager;
    QList<_IMAGE> images2dl;

    void parseHTML(void);
    bool addImage(_IMAGE img);
    int getNextImage(QString* s);

    bool downloading;
    bool useOriginalFilename;
    int activeDownloads;
    int maxDownloads;

private slots:
    void replyFinished(QNetworkReply*);
    void download(bool b);
    int setCompleted(QString s);
    void dlProgress(qint64 b, qint64 t) {qDebug() << QString("%1 of %2").arg(b).arg(t);}
    void replyError(QNetworkReply::NetworkError e) {qDebug() << "reply error " << e;}

public slots:
    void setURI(QString pURI) { uri = QUrl(pURI); sURI = pURI;}
    void setSavePath(QString pSavePath) { savePath = pSavePath; }
    void start(void);
    void stop(void);
    void setUseOriginalFilename(int);
    void setValues(QString s) {values = s;}

signals:
    void finished(void);
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
