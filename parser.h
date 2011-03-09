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

public slots:
    void setURI(QString pURI) { uri = QUrl(pURI); sURI = pURI;}
    void setSavePath(QString pSavePath) { savePath = pSavePath; }
    void start(void);
    void stop(void);
    void setUseOriginalFilename(int);

signals:
    void finished(void);
    void downloadsAvailable(bool);
    void totalCountChanged(int);
    void downloadedCountChanged(int);
    void fileFinished(QString);
    void error(int);
    void threadTitleChanged(QString);
    void tabTitleChanged(QString);
};

#endif // PARSER_H
