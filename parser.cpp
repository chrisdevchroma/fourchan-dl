#include "parser.h"

Parser::Parser(QObject *parent) :
    QObject(parent)
{
    downloading = false;

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
                f.open(QIODevice::ReadWrite);

                f.write(r->readAll());
                f.close();

                emit fileFinished(savePath+"/"+res.at(1)+res.at(2));

                if (0 != setCompleted(requestURI))
                {
//                    qDebug() << "Oh Oh!";
                }
            }

            QString imgURI;

            if (0 != getNextImage(&imgURI)) {
                manager->get(QNetworkRequest(QUrl(imgURI)));
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
    }
}

void Parser::parseHTML() {
    QStringList res;
    QRegExp rx("<a href=\"http://images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"filetitle\">([^<]+)</span>");
    bool imagesAdded;
    int pos;
    _IMAGE i;

    imagesAdded = false;
    pos = 0;
    i.downloaded = false;
    i.requested = false;

    while (pos > -1) {
        pos = rx.indexIn(html, pos+1);
        res = rx.capturedTexts();

        i.largeURI = "http://images.4chan.org/"+res.at(1);
        i.thumbURI = res.at(2);

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

    if (!imagesAdded)
        emit finished();
}

void Parser::start(void) {
    if (uri.isValid()) {
        manager->get(QNetworkRequest(uri));
    }
}

void Parser::download(bool b) {
    if (b) {
        QString imgURI;

        if (0 != getNextImage(&imgURI)) {
            manager->get(QNetworkRequest(QUrl(imgURI)));
        }

        downloading = true;
    }
}

void Parser::stop(void) {

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
        images2dl.append(img);

        emit downloadsAvailable(true);
        emit totalCountChanged(getTotalCount());
    }

    return !alreadyInList;
}

int Parser::getNextImage(QString* s) {
    int i;
    _IMAGE tmp;
    QFile f;

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

            if (getDownloadedCount() == getTotalCount()) {
                downloading = false;
                emit finished();
            }
            return 0;
        }
    }

    return -1;
}

int Parser::getTotalCount(void) {
    return images2dl.count();
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
