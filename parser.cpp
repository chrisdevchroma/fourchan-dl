#include "parser.h"

Parser::Parser(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    rescheduleTimer = new QTimer();

    requestHandler = new RequestHandler(this);

    connect(requestHandler, SIGNAL(response(QUrl,QByteArray)),
            this, SLOT(processRequestResponse(QUrl,QByteArray)));

    connect(requestHandler, SIGNAL(responseError(QUrl,int)),
            this, SLOT(handleError(QUrl,int)));

    downloading = false;
    useOriginalFilename = false;
    activeDownloads = 0;

    maxDownloads = 2;
    rescheduleTimer->setSingleShot(true);
    setTimerInterval(30000);

    connect(this, SIGNAL(downloadsAvailable(bool)),
            this, SLOT(download(bool)));

    connect(rescheduleTimer, SIGNAL(timeout()), this, SLOT(processSchedule()));
}

void Parser::processRequestResponse(QUrl url, QByteArray ba) {
    QString requestURI;

    requestURI = url.toString();
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

            f.write(ba);
            f.close();

            emit fileFinished(f.fileName());

            setCompleted(requestURI, f.fileName());
        }

    }
    else {
        html = ba;

        if (ba.contains("<title>4chan - Banned</title>"))
            emit error(999);
        else
            parseHTML();
    }
}

void Parser::parseHTML() {
    QStringList res;
    QRegExp rx("<span title=\"([^\"]+)\">[^>]+</span>\\)</span><br><a href=\"http://images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp boardPage("<a href=\"res/(\\d+)\">Reply</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"filetitle\">([^<]+)</span>");
    bool imagesAdded;
    bool pageIsFrontpage;
    QStringList imageThreads;
    int pos;
    _IMAGE i;

    emit tabTitleChanged("parsing HTML");
    imagesAdded = false;
    pos = 0;
    i.downloaded = false;
    i.requested = false;
    pageIsFrontpage = false;

    while (pos > -1) {
        pos = boardPage.indexIn(html, pos+1);
        res = boardPage.capturedTexts();

        if (res.at(1) != "") {
            pageIsFrontpage=true;
            imageThreads << res.at(1);
        }

    }

    pos = 0;
    while (pos > -1) {
        pos = rx.indexIn(html, pos+1);
        res = rx.capturedTexts();
        QUrl temp = QUrl::fromEncoded(res.at(1).toAscii());

        i.originalFilename = temp.toString();
        i.largeURI = "http://images.4chan.org/"+res.at(2);
        i.thumbURI = res.at(3);

        if (pos != -1) {
            if (addImage(i)) {
                imagesAdded = true;
            }
        }
    }

    pos = 0;
    while (pos > -1) {
        pos = rxTitle.indexIn(html,pos+1);
        res = rxTitle.capturedTexts();

        if (res.at(1) != "")
            emit threadTitleChanged(res.at(1));
    }

    if (pageIsFrontpage) {
        // Open new Tab for each thread
        QStringList sl;
        QStringList splittedURI;

        splittedURI = sURI.split("/");
        sl = values.split(";;");

        foreach (QString uri, imageThreads) {
            sl.replace(0, QString("http://boards.4chan.org/%1/res/%2").arg(splittedURI.at(3)).arg(uri));
            emit createTabRequest(sl.join(";;"));
        }
        emit closeTabRequest();
    }
    else {
        if (!imagesAdded && (getDownloadedCount() == getTotalCount())){
            download(false);
            emit downloadFinished();
            emit tabTitleChanged("finished");
        } else {
            emit downloadsAvailable(true);
            emit tabTitleChanged(QString("%1/%2").arg(getDownloadedCount()).arg(getTotalCount()));
        }
    }

    html.clear();
}


void Parser::createSupervisedDownload(QUrl url) {
    if (url.isValid()) {
        requestHandler->request(url);
    }
}

void Parser::removeSupervisedDownload(QUrl url) {
    requestHandler->cancel(url);
}

void Parser::startDownload(void) {
    createSupervisedDownload(uri);
}

void Parser::download(bool b) {
    if (b) {
        QString imgURI;

        downloading = true;

        while (getNextImage(&imgURI) != 0)
                createSupervisedDownload(QUrl(imgURI));
    } else {
        downloading = false;
    }
}

void Parser::stopDownload(void) {
    _IMAGE tmp;

    download(false);                // Prevent new requests
    requestHandler->cancelAll();    // Cancel pending downloads

    // Reset requested, yet not finished, downloads
    for (int i=0; i<images2dl.length(); i++) {
        if ((images2dl.at(i).downloaded==false) && (images2dl.at(i).requested == true)) {
            tmp = images2dl.at(i);
            tmp.requested = false;
            images2dl.replace(i, tmp);
        }
    }
}

bool Parser::addImage(_IMAGE img) {
    int i,k;
    bool alreadyInList;
    bool fileExists;

    if (blacklisted(img.largeURI)) {
//        qDebug() << "Preventing download of blacklisted image " << img.largeURI;
        alreadyInList = true;
    }
    else {
        alreadyInList = false;
        fileExists = false;
        k = 2;
        for (i=0; i<images2dl.length(); i++) {
            if (images2dl.at(i).largeURI == img.largeURI) {
                alreadyInList = true;
                break;
            }

            if (images2dl.at(i).originalFilename == img.originalFilename) {
                QStringList tmp;

                tmp = img.originalFilename.split(QRegExp("\\(\\d+\\)"));
                if  (tmp.count() > 1) // Already has a number in brackets in filename
                    img.originalFilename = QString("%1(%2)%3").arg(tmp.at(0)).
                                           arg(k++).
                                           arg(tmp.at(1));
                else
                    img.originalFilename = img.originalFilename.replace("."," (1).");
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
                fileExists = true;
                img.savedAs = f.fileName();
            }

            images2dl.append(img);
            emit totalCountChanged(getTotalCount());

            if (fileExists) {
                emit downloadedCountChanged(getDownloadedCount());
                emit fileFinished(f.fileName());
            }
        }
    }

    return (!alreadyInList && !fileExists);
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

int Parser::setCompleted(QString uri, QString filename) {
    int i;
    _IMAGE tmp;

    for (i=0; i<images2dl.length(); i++) {
        if (images2dl.at(i).largeURI == uri) {
            tmp = images2dl.at(i);
            tmp.downloaded = true;
            tmp.savedAs = filename;

            images2dl.replace(i,tmp);

            emit downloadedCountChanged(getDownloadedCount());
            emit tabTitleChanged(QString("%1/%2").arg(getDownloadedCount()).arg(getTotalCount()));

            if (isFinished()) {
                download(false);
                emit downloadFinished();
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
    bool checkOrgFilename;
    bool checkLargeURI;

    f = filename.right(filename.count()-filename.lastIndexOf("/")-1);

    for (i=0; i<images2dl.count(); i++) {
        checkOrgFilename = images2dl.at(i).originalFilename.endsWith(f);
        checkLargeURI = images2dl.at(i).largeURI.endsWith(f);
        if ((useOriginalFilename && checkOrgFilename)
            || (!useOriginalFilename && checkLargeURI)) {
            tmp = images2dl.at(i);

            tmp.downloaded = false;
            tmp.requested = false;

            images2dl.replace(i, tmp);

            break;
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

bool Parser::getUrlOfFilename(QString filename, QString * url) {
    bool ret;
    int i;

    ret = false;

    for (i=0; i<images2dl.count(); i++) {
        if (images2dl.at(i).savedAs == filename) {
            *url = images2dl.at(i).largeURI;
            ret = true;
            break;
        }
    }

    return ret;
}

void Parser::handleError(QUrl url, int err) {
    switch (err) {
    case 404:
    case 203:
        // Only emit signal on pages not on images to prevent closing of tab just because one image is missing
         if (url.toString().indexOf(QRegExp("(\\.jpg|\\.gif|\\.jpeg|\\.png)", Qt::CaseInsensitive)) == -1)
            emit error(404);
          else
             setCompleted(url.toString(),"");   // 404'ed images are set "completed"
        break;
    case 301:
        qDebug() << QString("%1 - service unavailable").arg(url.toString());
        reschedule(url.toString());
        break;
    case 202:
        emit message(QString("Server replied: Access denied! - Maybe a Proxy issue."));
        debug_out("202: Access denied", 1);
        break;
    case 5:
        // Download was aborted -> reschedule
        qDebug() << QString("%1 - timed out").arg(url.toString());
        reschedule(url.toString());

        break;
    default:
        qDebug() << url.toString() << "- Unhandled error " << err;
        break;
    }
}

void Parser::reschedule(QString s) {
    QRegExp rx("\\/(\\w+)(\\.jpg|\\.gif|\\.png)", Qt::CaseInsensitive, QRegExp::RegExp2);
    QStringList res;
    int pos;

    pos = rx.indexIn(s);
    res = rx.capturedTexts();
//    qDebug() << "Rescheduling " << s;
    emit message(QString("Rescheduling %1").arg(s));

    if (pos != -1) {
        if (rescheduled.contains(s))
            rescheduled.removeAll(s);

        rescheduled.append(s);

        if (!rescheduleTimer->isActive())
            rescheduleTimer->start();

        removeSupervisedDownload(QUrl(s));
    }
    else {
        // Requested URL was no image -> reschedule immediately
        createSupervisedDownload(QUrl(s));
    }

}

void Parser::downloadAborted(QString uri) {
    removeSupervisedDownload(uri);
    createSupervisedDownload(uri);
}

void Parser::processSchedule() {
    _IMAGE tmp;
//    qDebug() << "triggering download";
    for (int i=0; i<images2dl.count(); i++) {
        if (rescheduled.contains(images2dl.at(i).largeURI)) {
            tmp = images2dl.at(i);

            tmp.requested = false;
            tmp.downloaded = false;
            images2dl.replace(i, tmp);
            rescheduled.removeAll(tmp.largeURI);
        }
    }
    download(true);
}

void Parser::setTimerInterval(int msec) {
    if (rescheduleTimer == 0)
        qDebug() << "OhOh!";
    else
        rescheduleTimer->setInterval(msec);
}

QStringList Parser::getDownloadedFiles() {
    QStringList ret;

    ret.empty();

    for (int i=0; i<images2dl.count(); i++) {
        if (images2dl.at(i).downloaded)
            ret << images2dl.at(i).savedAs;
    }

    return ret;
}

bool Parser::isFinished() {
    bool ret;

    ret = false;

    if (getDownloadedCount() == getTotalCount())
        ret=true;

    return ret;
}
