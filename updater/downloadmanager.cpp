#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    output = new QTextStream(stdout);
    manager = new QNetworkAccessManager();
    cookies = new QNetworkCookieJar();
    manager->setCookieJar(cookies);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

}

void DownloadManager::replyFinished(QNetworkReply* r) {
    QString requestURI;
    QString tmpFilename;
    QUuid uuid;
    QFile f;

    if (r->bytesAvailable() < r->header(QNetworkRequest::ContentLengthHeader).toLongLong()) {
//        p(QString("Received only partial data of %1. Reinitiating download.").arg(r->url().toString()));

        manager->get(QNetworkRequest(r->url()));
        r->deleteLater();
    }
    else {
        if (r->isFinished()) {
            QString mimeType;
            QString redirectTo;

            redirectTo = r->header(QNetworkRequest::LocationHeader).toString();
            mimeType = r->header(QNetworkRequest::ContentTypeHeader).toString();
            requestURI = r->request().url().toString();

            if (redirectTo != "") {
                FileUpdate fu;
                for (int i=0; i<updateList.count(); i++) {
                    if (updateList.at(i).uri == requestURI) {
                        fu = updateList.at(i);
                        fu.uri = redirectTo;
                        updateList.replace(i, fu);

                        break;
                    }
                }

                p("Following redirect to "+redirectTo);
                initiateDownload();
            }
            else if (mimeType.startsWith("text/html")) {
                p("Received HTML content. Don't know what to do.");
                emit error("There is a problem downloading " + r->url().toString() + ". Exiting.");
                exit(1);
            }
            else {
                p("Finished download "+requestURI);
                tmpFilename = uuid.createUuid().toString()+".file";

                f.setFileName(tmpFilename);
                f.open(QIODevice::ReadWrite);
                f.write(r->readAll());
                f.close();
                p("Saved as "+tmpFilename);

                for (int i=0; i<updateList.count(); i++) {
                    if (updateList.at(i).uri == requestURI) {
                        FileUpdate fu;

                        fu = updateList.at(i);
                        fu.tmpFilename = tmpFilename;

                        updateList.replace(i, fu);
                        break;
                    }
                }
                initiateDownload();
            }
        }
    }
}

void DownloadManager::startDownload(QList<FileUpdate> l) {
    updateList = l;
    initiateDownload();
}

void DownloadManager::initiateDownload() {
    QNetworkReply* nr;
    QString uri;
    bool downloadsAvailable;

    downloadsAvailable = false;

    for (int i=0; i<updateList.count(); i++) {
        if (updateList.at(i).tmpFilename == "") {
            uri = updateList.at(i).uri;
            downloadsAvailable = true;
            break;
        }
    }

    if (downloadsAvailable) {
        QNetworkRequest req;

        req.setUrl(QUrl(uri));
        req.setAttribute(QNetworkRequest::CookieSaveControlAttribute, QNetworkRequest::Automatic);
        req.setRawHeader("User-Agent", "Wget/1.12");
        nr = manager->get(req);

        connect(nr,SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));

        p("Downloading "+uri);
    }
    else {
        emit downloadsFinished(updateList);
    }
}

void DownloadManager::replyError(QNetworkReply::NetworkError e) {
    p("Reply error "+QString::number(e));
}

void DownloadManager::p(QString msg) {
    *output << "DownloadManager: " << msg << endl;
    output->flush();
}
