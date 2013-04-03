#include "blacklist.h"

BlackList::BlackList(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer();
    manager = new QNetworkAccessManager();
    blackListFile = new QFile("black.list");
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    list.clear();

    loadBlackList();
    loadSettings();

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(timer, SIGNAL(timeout()), this, SLOT(cleanUp()));

    timer->setInterval(10*60*1000); // Every 10 minutes
}

BlackList::~BlackList () {
    saveBlackList();
}

void BlackList::loadBlackList() {
    QString line;
    blackListFile->open(QIODevice::ReadOnly);

    if (blackListFile->isReadable()) {
        while(!blackListFile->atEnd()) {
            line = blackListFile->readLine();
            list.append(line.simplified());
        }
        cleanUp();
    }

    blackListFile->close();
}

void BlackList::saveBlackList() {
    blackListFile->remove();
    blackListFile->open(QIODevice::WriteOnly);
    foreach (QString s, list) {
        blackListFile->write(s.toLocal8Bit());
        blackListFile->write("\n");
    }
    blackListFile->close();
}

void BlackList::add(QString uri) {
    if (!contains(uri)) {
        list.append(uri);
        QLOG_INFO() << "BlackList :: Blacklisted image " << uri;
        saveBlackList();
    }
}

void BlackList::remove(QString uri) {
    list.removeAll(uri);
}

bool BlackList::contains(QString uri) {
    bool ret;

    if (useBlackList)
        ret = list.contains(uri, Qt::CaseInsensitive);
    else
        ret = false;

    return ret;
}

void BlackList::cleanUp() {
    foreach (QString uri, list) {
        manager->head(QNetworkRequest(QUrl(uri)));
    }
}

void BlackList::replyFinished(QNetworkReply* r) {
    switch (r->error()) {
    //case 404:
    case 203:
        remove(r->url().toString());
        break;

    default:
        break;
    }

    r->deleteLater();
}

void BlackList::loadSettings() {
    useBlackList = settings->value("blacklist/use_blacklist", true).toBool();
    interval = settings->value("blacklist/blacklist_check_interval", 600).toInt();
    interval *= 1000;

    timer->stop();
    timer->setInterval(interval);
    timer->start();
}
