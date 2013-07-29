#ifndef IMAGETHREAD_H
#define IMAGETHREAD_H

#include <QObject>
#include <QTimer>
#include <QSettings>
#include <QUrl>
#include "blacklist.h"
#include "requesthandler.h"
#include "ParserPluginInterface.h"
#include "pluginmanager.h"

extern PluginManager* pluginManager;

class ImageThread : public QObject
{
    Q_OBJECT
public:
    explicit ImageThread(QObject *parent = 0);
    QString getValues(void);
    void setValues(QString);
    void setBlackList(BlackList* bl);
    int getTotalImagesCount() {return images.count();}
    int getDownloadedImagesCount();
    QString getUrl() {return _sUrl;}

private:
    QTimer* timer;
    QList<int> timeoutValues;
    QList<_IMAGE> images;
    QSize iconSize;
    QSettings* settings;
    BlackList* _blackList;
    QString _status;
    RequestHandler* requestHandler;
    ParserPluginInterface* iParser;
    QObject* oParser;
    QString _sUrl;
    QString _sSavepath;

    bool _running;
    bool _closeWhenFinished;
    bool _downloading;
    bool _rescan;
    int  _rescanInterval;
    bool _saveWithOriginalFilename;

    void setStatus(QString s);
    void createSupervisedDownload(QUrl);
    void removeSupervisedDownload(QUrl);
    bool getUrlOfFilename(QString filename, QString * url);
    bool isDownloadFinished();
    bool getNextImage(QString* s);
    bool selectParser(QUrl url=QUrl());
    void mergeImageList(QList<_IMAGE> list);
    bool addImage(_IMAGE img);
    QString getSavepath();

private slots:
    void errorHandler(QUrl, int);
    void messageHandler(QString);
    void processCloseRequest();
    void triggerRescan(void);
    void startDownload(void);
    void stopDownload(void);
    void download(bool b);
    void processRequestResponse(QUrl url, QByteArray ba, bool cached);
    void setCompleted(QString uri, QString filename);
    bool isImage(QUrl);

public slots:
    void start(void);
    void stop(void);
    bool checkForExistingThread(QString s="");

signals:
    void finished(void);
    void errorMessage(QString);
    void closeRequest(ImageThread*, int);
    void createThreadRequest(QString);
    void removeFiles(QStringList);
};

#endif // IMAGETHREAD_H
