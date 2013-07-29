#include "threadhandler.h"

ThreadHandler::ThreadHandler(QObject *parent) :
    QObject(parent)
{
    aui = new ApplicationUpdateInterface(this);
    requestHandler = new RequestHandler(this);
    blackList = new BlackList(this);

    thumbnailRemoverThread = new QThread();
    thumbnailRemover = new ThumbnailRemover();
    thumbnailRemover->moveToThread(thumbnailRemoverThread);
    connect(thumbnailRemoverThread, SIGNAL(started()), thumbnailRemover, SLOT(removeOutdated()));
    thumbnailRemoverThread->start(QThread::LowPriority);

    connect(this, SIGNAL(removeFiles(QStringList)), thumbnailRemover, SLOT(removeFiles(QStringList)));

    settings = new QSettings("settings.ini", QSettings::IniFormat);
    loadSettings();

    connect(requestHandler, SIGNAL(response(QUrl,QByteArray,bool)), this, SLOT(processRequestResponse(QUrl,QByteArray,bool)));
    connect(requestHandler, SIGNAL(responseError(QUrl,int)), this, SLOT(handleRequestError(QUrl,int)));

//    connect(downloadManager, SIGNAL(error(QString)), ui->statusBar, SLOT(showMessage(QString)));
//    connect(downloadManager, SIGNAL(finishedRequestsChanged(int)), this, SLOT(updateDownloadProgress()));
//    connect(downloadManager, SIGNAL(totalRequestsChanged(int)), this, SLOT(updateDownloadProgress()));

    connect(aui, SIGNAL(connectionEstablished()), this, SLOT(updaterConnected()));
    connect(aui, SIGNAL(updateFinished()), this, SLOT(updateFinished()));
    connect(aui, SIGNAL(updaterVersionSent(QString)), this, SLOT(setUpdaterVersion(QString)));

    createComponentList();

#ifdef __DEBUG__
    createSupervisedDownload(QUrl("file:d:/Qt/fourchan-dl/webupdate.xml"));
#else
    createSupervisedDownload(QUrl("http://www.sourceforge.net/projects/fourchan-dl/files/webupdate/webupdate.xml/download"));
#endif

    autosaveTimer = new QTimer(this);
    autosaveTimer->setInterval(1000*60*10);     // 10 Minutes
    autosaveTimer->setSingleShot(false);
    connect(autosaveTimer, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

void ThreadHandler::loadSettings() {
    settings->beginGroup("options");
        autoClose = settings->value("automatic_close", false).toBool();
        maxDownloads = settings->value("concurrent_downloads", 1).toInt();
    settings->endGroup();

    settings->beginGroup("network");
    QNetworkProxy proxy;

    if (settings->value("use_proxy", false).toBool()) {
        proxy.setType((QNetworkProxy::ProxyType)(settings->value("proxy_type", QNetworkProxy::HttpProxy).toInt()));
        proxy.setHostName(settings->value("proxy_hostname", "").toString());
        proxy.setPort(settings->value("proxy_port", 0).toUInt());
        if (settings->value("proxy_auth", false).toBool()) {
            proxy.setUser(settings->value("proxy_user", "").toString());
            proxy.setPassword(settings->value("proxy_pass", "").toString());
        }
    }
    else {
        proxy.setType(QNetworkProxy::NoProxy);
    }

    QNetworkProxy::setApplicationProxy(proxy);

    settings->endGroup();
}

void ThreadHandler::saveSettings() {
    QLOG_DEBUG() << "ThreadHandler :: Saving threads";

    settings->beginGroup("tabs");
        settings->setValue("count", threadList.count());

        for (int i=0; i<threadList.count(); i++) {
            settings->setValue(QString("tab%1").arg(i), threadList.at(i)->getValues());
        }
    settings->endGroup();

    settings->sync();
}

void ThreadHandler::restoreThreads() {
    int threadCount;

    out << "Reopening threads\n";

    threadCount = settings->value("tabs/count",0).toInt();
    QLOG_TRACE() << "ThreadHandler :: restoring Threads";

    ImageThread* it;

    if (threadCount > 0) {
        for (int i=0; i<threadCount; i++) {
            it = addThread();

            it->setValues(
                    settings->value(QString("tabs/tab%1").arg(i), ";;;;0;;every 30 seconds;;0").toString()
                    );
            out << " opening " << it->getUrl() << "\n";
        }
    }
    else {
        QLOG_WARN() << "ThreadHandler :: No threads available to restore";
        emit threadListEmpty();
    }
    out.flush();
}

ImageThread* ThreadHandler::addThread() {
    ImageThread* ret;

    ret = new ImageThread(this);

    threadList.append(ret);
    ret->setBlackList(blackList);

    connect(ret, SIGNAL(closeRequest(ImageThread*,int)), this, SLOT(processCloseRequest(ImageThread*,int)));
    connect(ret, SIGNAL(createThreadRequest(QString)), this, SLOT(createThread(QString)));
    QLOG_TRACE() << "ThreadHandler :: adding thread";

    return ret;
}

void ThreadHandler::createThread(QString thread_settings) {
    ImageThread* it;

    it = addThread();
    it->setValues(thread_settings);
    out << "Opening thread " << it->getUrl() << "\n";
    out.flush();
}

void ThreadHandler::closeThread(ImageThread* it) {
    QLOG_TRACE() << "ThreadHandler :: closing thread";
    out << "Closing thread " << it->getUrl() << "\n";
    out.flush();

    it->stop();
    it->deleteLater();
    threadList.removeAll(it);
}

void ThreadHandler::startAll() {
    for (int i=0; i < threadList.count(); i++) {
        threadList.at(i)->start();
    }
}

void ThreadHandler::stopAll() {
    for (int i=0; i < threadList.count(); i++) {
        threadList.at(i)->stop();
    }
}

void ThreadHandler::createComponentList() {

}

void ThreadHandler::createSupervisedDownload(QUrl url) {
    if (url.isValid()) {
        requestHandler->request(url, 0);
    }
}

void ThreadHandler::processRequestResponse(QUrl url, QByteArray ba, bool cached) {

}

void ThreadHandler::updaterConnected() {

}

void ThreadHandler::updateFinished() {

}

void ThreadHandler::setUpdaterVersion(QString v) {

}

void ThreadHandler::aboutToQuit() {
    downloadManager->pauseDownloads();
    saveSettings();
}

void ThreadHandler::processCloseRequest(ImageThread* it, int reason) {
    closeThread(it);
}
