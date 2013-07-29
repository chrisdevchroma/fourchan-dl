#ifndef THREADHANDLER_H
#define THREADHANDLER_H

#include <QObject>
#include "imagethread.h"
#include "applicationupdateinterface.h"
#include "blacklist.h"
#include "requesthandler.h"
#include "thumbnailremover.h"

extern DownloadManager* downloadManager;
extern QTextStream in;
extern QTextStream out;

class ThreadHandler : public QObject
{
    Q_OBJECT
public:
    explicit ThreadHandler(QObject *parent = 0);
    void restoreThreads();

private:
    QList<ImageThread*> threadList;
    QSettings* settings;
    ApplicationUpdateInterface* aui;
    BlackList* blackList;
    bool autoClose;
    int maxDownloads;
    RequestHandler* requestHandler;
    QThread* thumbnailRemoverThread;
    ThumbnailRemover* thumbnailRemover;
    QTimer* autosaveTimer;

    void checkVersion(QString ver);
    void checkForUpdates(QString xml);
    bool checkIfNewerVersion(QString _new, QString _old);

    void createSupervisedDownload(QUrl);
    void removeSupervisedDownload(QUrl);

    void createComponentList();
    void newComponentsAvailable();

    ImageThread* addThread();

public slots:
    void loadSettings(void);
    void saveSettings(void);
    void closeThread(ImageThread*);
    void createThread(QString);

    void processCloseRequest(ImageThread*, int);
    void startAll(void);
    void stopAll(void);
    void processRequestResponse(QUrl url, QByteArray ba, bool cached);
    void updaterConnected();
    void updateFinished();
    void setUpdaterVersion(QString);
    void aboutToQuit();

signals:
    void removeFiles(QStringList);
    void threadListEmpty();
};

#endif // THREADHANDLER_H
