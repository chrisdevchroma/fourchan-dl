#include "parser.h"

Parser::Parser(QObject *parent) :
    QObject(parent)
{
    downloading = false;
    useOriginalFilename = false;
    activeDownloads = 0;
    maxDownloads = 2;

    manager = new QNetworkAccessManager();

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    connect(this, SIGNAL(downloadsAvailable(bool)),
            this, SLOT(download(bool)));
}

void Parser::replyFinished(QNetworkReply* r) {
    QString requestURI;

    if (r->isFinished()) {
        requestURI = r->request().url().toString();

        if (requestURI.indexOf(QRegExp("(\\.jpg|\\.gif|\\.png)", Qt::CaseInsensitive)) != -1) {
            QFile f;
            QRegExp rx("\\/(\\w+)(\\.jpg|\\.gif|\\.png)", Qt::CaseInsensitive, QRegExp::RegExp2);
            QStringList res;
            int pos;

            pos = 0;

            pos = rx.indexIn(requestURI);
            res = rx.capturedTexts();

            if (pos != -1) {
                f.setFileName(savePath+"/"+res.at(1)+res.at(2));

                if (useOriginalFilename) {
                    _IMAGE tmp;

                    for (int i=0; i<images2dl.count(); i++) {
                        if (images2dl.at(i).largeURI.endsWith("/"+res.at(1)+res.at(2))) {
                            tmp = images2dl.at(i);

                            f.setFileName(savePath+"/"+tmp.originalFilename);
                            break;
                        }
                    }
                }

                f.open(QIODevice::ReadWrite);

                f.write(r->readAll());
                f.close();

                emit fileFinished(f.fileName());
                activeDownloads--;

                setCompleted(requestURI);
            }

            for (int i=activeDownloads; i<maxDownloads; i++) {
                QString imgURI;

                if (0 != getNextImage(&imgURI)) {
                    manager->get(QNetworkRequest(QUrl(imgURI)));
                    activeDownloads++;
                }
            }
        }
        else {
            html = r->readAll();

            if (html.contains("4chan - 404")) {
                emit error(404);
            }
            else
                parseHTML();
        }

        r->deleteLater();
    }
}

void Parser::parseHTML() {
    QStringList res;
                          //    QRegExp rx("<a href=\"http://images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rx(">([^>]+)</span>\\)</span><br><a href=\"http://images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);

    QRegExp rxTitle("<span class=\"filetitle\">([^<]+)</span>");
    bool imagesAdded;
    int pos;
    _IMAGE i;

    emit tabTitleChanged("parsing HTML");
    imagesAdded = false;
    pos = 0;
    i.downloaded = false;
    i.requested = false;

    while (pos > -1) {
        pos = rx.indexIn(html, pos+1);
        res = rx.capturedTexts();

        i.originalFilename = res.at(1);
        i.largeURI = "http://images.4chan.org/"+res.at(2);
        i.thumbURI = res.at(3);

        if (pos != -1){
            if (addImage(i))
                imagesAdded = true;
        }
    }

    pos = 0;
    while (pos > -1) {
        pos = rxTitle.indexIn(html,pos+1);
        res = rxTitle.capturedTexts();

        if (res.at(1) != "")
            emit threadTitleChanged(res.at(1));
    }

    if (!imagesAdded){
        emit finished();
        emit tabTitleChanged("finished");
    } else {
        emit downloadsAvailable(true);
    }
}

void Parser::start(void) {
    if (uri.isValid()) {
        manager->get(QNetworkRequest(uri));
    }
}

void Parser::download(bool b) {
    if (b) {
        QString imgURI;

        downloading = true;
        emit tabTitleChanged("downloading");

        for (int i=activeDownloads; i<maxDownloads; i++) {
            if (0 != getNextImage(&imgURI)) {
                manager->get(QNetworkRequest(QUrl(imgURI)));
                activeDownloads++;
            }
        }

    } else {
        downloading = false;
    }
}

void Parser::stop(void) {
    download(false);
}

bool Parser::addImage(_IMAGE img) {
    int i;
    bool alreadyInList;

    alreadyInList = false;
    for (i=0; i<images2dl.length(); i++) {
        if (images2dl.at(i).largeURI == img.largeURI) {
            alreadyInList = true;
            break;
        }
    }

    if (!alreadyInList) {
        // Check if already downloaded
        QFile f;

        if (useOriginalFilename)
            f.setFileName(savePath+"/"+img.originalFilename);
        else {
            QRegExp rx("\\/(\\w+)(\\.jpg|\\.gif|\\.png)", Qt::CaseInsensitive, QRegExp::RegExp2);
            QStringList res;
            int pos;

            pos = 0;

            pos = rx.indexIn(img.largeURI);
            res = rx.capturedTexts();

            if (pos != -1) {
                f.setFileName(savePath+"/"+res.at(1)+res.at(2));
            }
        }

        if (f.exists()) {
            img.downloaded = true;

            emit downloadedCountChanged(getDownloadedCount());
            emit fileFinished(f.fileName());
        }
        images2dl.append(img);

        emit totalCountChanged(getTotalCount());
    }

    return !alreadyInList;
}

int Parser::getNextImage(QString* s) {
    int i;
    _IMAGE tmp;
    QFile f;

    if (downloading) {
        for (i=0; i<images2dl.length(); i++) {
            if (!images2dl.at(i).downloaded && !images2dl.at(i).requested) {
                tmp = images2dl.at(i);
                tmp.requested = true;
                images2dl.replace(i,tmp);

                // Check if file already exists in destination dir
                QRegExp rx("\\/(\\w+)(\\.jpg|\\.gif|\\.png)", Qt::CaseInsensitive, QRegExp::RegExp2);
                QStringList res;
                int pos;

                pos = 0;

                pos = rx.indexIn(tmp.largeURI);
                res = rx.capturedTexts();

                if (pos != -1) {
                    f.setFileName(savePath+res.at(0));
                }

                if (f.exists()) {
                    tmp.downloaded = true;
                    images2dl.replace(i, tmp);
                }
                else {
                    *s = tmp.largeURI;
                    return 1;
                }
            }
        }
    }

    return 0;
}

int Parser::setCompleted(QString s) {
    int i;
    _IMAGE tmp;

    for (i=0; i<images2dl.length(); i++) {
        if (images2dl.at(i).largeURI == s) {
            tmp = images2dl.at(i);
            tmp.downloaded = true;

            images2dl.replace(i,tmp);

            emit downloadedCountChanged(getDownloadedCount());
            emit tabTitleChanged(QString("%1/%2").arg(getDownloadedCount()).arg(getTotalCount()));

            if (getDownloadedCount() == getTotalCount()) {
                downloading = false;
                emit finished();
                emit tabTitleChanged("finished");
            }
            return 0;
        }
    }

    return -1;
}

int Parser::getTotalCount(void) {
    int ret;

    ret = images2dl.count();

    return ret;
}

int Parser::getDownloadedCount(void) {
    int i;
    int ret;

    ret = 0;

    for (i=0; i<images2dl.count(); i++) {
        if (images2dl.at(i).downloaded)
            ret++;
    }

    return ret;
}

void Parser::reloadFile(QString filename) {
    QString f;
    _IMAGE tmp;
    int i;

    f = filename.right(filename.lastIndexOf("/"));

    for (i=0; i<images2dl.count(); i++) {
        if (images2dl.at(i).largeURI.endsWith(f)) {
            tmp = images2dl.at(i);

            tmp.downloaded = false;
            tmp.requested = false;

            images2dl.replace(i, tmp);
        }
    }

    download(true);
}

void Parser::setUseOriginalFilename(int i) {
    if (i == Qt::Checked)
        useOriginalFilename = true;
    else
        useOriginalFilename = false;
}

void Parser::setMaxDownloads(int i) {
    maxDownloads = i;
}
