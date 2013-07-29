#include "imagethread.h"

ImageThread::ImageThread(QObject *parent) :
    QObject(parent)
{
    requestHandler = new RequestHandler(this);
    iParser = 0;
    oParser = 0;

    timer = new QTimer(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    _closeWhenFinished = false;
    _downloading = false;
    _blackList = 0;

    connect(requestHandler, SIGNAL(responseError(QUrl, int)), this, SLOT(errorHandler(QUrl, int)));
    connect(requestHandler, SIGNAL(response(QUrl, QByteArray,bool)), this, SLOT(processRequestResponse(QUrl, QByteArray, bool)));

    connect(timer, SIGNAL(timeout()), this, SLOT(triggerRescan()));
}

void ImageThread::setValues(QString values) {
    QStringList list;

    list = values.split(";;");

    _sUrl = list.at(0);
    _sSavepath = list.at(1);

    if (list.value(2).toInt() == 0) {
        _rescan = false;
    } else {
        _rescan = true;
        _rescanInterval = list.at(3).toInt();
    }

    _saveWithOriginalFilename = ((bool)list.value(4).toInt());

    if (list.value(5) == "1") {
        start();
    }
}

QString ImageThread::getValues() {
    QString ret;
    QStringList list;

    list << _sUrl;
    list << _sSavepath;
    list << QString("%1").arg(_rescan);
    list << QString("%1").arg(_rescanInterval);
    list << QString("%1").arg(_saveWithOriginalFilename);
    list << QString("%1").arg(_running);

    ret = list.join(";;");

    return ret;
}

void ImageThread::createSupervisedDownload(QUrl url) {
    if (url.isValid()) {
        requestHandler->request(url);
    }
}

void ImageThread::removeSupervisedDownload(QUrl url) {
    requestHandler->cancel(url);
}

void ImageThread::triggerRescan() {
    startDownload();
    timer->start();
}

bool ImageThread::getUrlOfFilename(QString filename, QString * url) {
    bool ret;
    int i;

    ret = false;

    for (i=0; i<images.count(); i++) {
        if (images.at(i).savedAs == filename) {
            *url = images.at(i).largeURI;
            ret = true;
            break;
        }
    }

    return ret;
}

bool ImageThread::isDownloadFinished() {
    bool ret;

    ret = false;

    if (getDownloadedImagesCount() == getTotalImagesCount()) {
        ret=true;
    }

    return ret;
}

bool ImageThread::getNextImage(QString* s) {
    int i;
    bool ret;
    _IMAGE tmp;
    QFile f;
    QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
    QStringList res;
    int pos;

    ret = false;

    if (_downloading) {
        for (i=0; i<images.length(); i++) {
            if (!images.at(i).downloaded && !images.at(i).requested) {
                tmp = images.at(i);
                tmp.requested = true;
                images.replace(i,tmp);

                // Check if file already exists in destination dir
                pos = 0;

                pos = rx.indexIn(tmp.largeURI);
                res = rx.capturedTexts();

                if (pos != -1) {
                    f.setFileName(getSavepath() + res.at(0));
                }

                if (f.exists()) {
                    tmp.downloaded = true;
                    images.replace(i, tmp);

                    setCompleted(tmp.largeURI, f.fileName());
                }
                else {
                    *s = tmp.largeURI;
                    ret = true;
                    break;
                }
            }
        }
    }

    return ret;
}

bool ImageThread::selectParser(QUrl url) {
    bool ret;
    ParserPluginInterface* tmp;

    if (url.isEmpty()) {
        url = QUrl(_sUrl);
    }

    tmp = pluginManager->getParser(url, &ret);
    if (ret) {
        oParser = tmp->createInstance();
        iParser = qobject_cast<ParserPluginInterface*>(oParser);
        iParser->setURL(url);
    }

    return ret;
}

void ImageThread::mergeImageList(QList<_IMAGE> list) {
    bool imagesAdded;

    imagesAdded = false;

    for (int i=0; i<list.count(); i++) {
        if(addImage(list.at(i)))
            imagesAdded = true;
    }

    if (imagesAdded) {
        QLOG_TRACE() << "ImageThread :: added images";
        download(true);
    }
    else {
        if (isDownloadFinished()) {
            download(false);
        }
    }
}

bool ImageThread::addImage(_IMAGE img) {
    int i,k;
    bool alreadyInList;
    bool fileExists;

    fileExists = false;

    if (_blackList->contains(img.largeURI)) {
        alreadyInList = true;
    }
    else {
        alreadyInList = false;
        k = 2;
        for (i=0; i<images.length(); i++) {
            if (images.at(i).largeURI == img.largeURI) {
                alreadyInList = true;
                break;
            }

            if (images.at(i).originalFilename == img.originalFilename) {
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
            if (_saveWithOriginalFilename)
                f.setFileName(getSavepath()+"/"+img.originalFilename);
            else {
                QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
                QStringList res;
                int pos;

                pos = 0;

                pos = rx.indexIn(img.largeURI);
                res = rx.capturedTexts();

                if (pos != -1) {
                    f.setFileName(getSavepath()+"/"+res.at(1)+res.at(2));
                }
            }

            if (f.exists()) {
                img.downloaded = true;
                fileExists = true;
                img.savedAs = f.fileName();
            }

            images.append(img);
        }
    }

    return (!alreadyInList && !fileExists);
}

void ImageThread::errorHandler(QUrl url, int err) {
    switch (err) {
    case 202:
    case 404:
        if (isImage(url)) {
            _blackList->add(url.toString());
            setCompleted(url.toString(), "");
        }
        else {
            // If there are still images in the list, wait until they finished (maybe they still exist)
            // else close immediately
            if (isDownloadFinished()) {
                stop();

                emit errorMessage("404 - Page not found");
                emit closeRequest(this, 404);
            }
            else {
                _closeWhenFinished = true;
            }
        }
        break;

    case 999:
        emit errorMessage("You are banned");
        break;

    default:
        QLOG_ERROR() << "ImageOverview :: Unhandled error (" << url.toString() << "," << err << ")";
        break;
    }
}

void ImageThread::messageHandler(QString m) {
    emit errorMessage(m);
}

void ImageThread::processCloseRequest() {
    stop();
    emit closeRequest(this, 0);
}


void ImageThread::startDownload(void) {
    createSupervisedDownload(QUrl(_sUrl));
}

void ImageThread::stopDownload(void) {
    _IMAGE tmp;

    download(false);                // Prevent new requests
    requestHandler->cancelAll();    // Cancel pending downloads

    // Reset requested, yet not finished, downloads
    for (int i=0; i<images.length(); i++) {
        if ((images.at(i).downloaded==false) && (images.at(i).requested == true)) {
            tmp = images.at(i);
            tmp.requested = false;
            images.replace(i, tmp);
        }
    }
}

void ImageThread::download(bool b) {
    if (b) {
        QString imgURI;

        _downloading = true;

        while (getNextImage(&imgURI) != 0)
                createSupervisedDownload(QUrl(imgURI));
    } else {
        _downloading = false;
    }
}

void ImageThread::processRequestResponse(QUrl url, QByteArray ba, bool cached) {
    QString requestURI;
    QList<_IMAGE>   imageList;
    QList<QUrl>     threadList;
    ParsingStatus   status;
    QString         path;
    qint64 bytesWritten;

    QLOG_TRACE() << "ImageThread :: processRequestResponse (" << url.toString() << ", " << QString(ba);

    requestURI = url.toString();
    path = url.path();

    if (isImage(url)) {
        QFile f;
        QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
        QStringList res;
        int pos;

        pos = 0;

        pos = rx.indexIn(requestURI);
        res = rx.capturedTexts();

        if (pos != -1) {
            f.setFileName(getSavepath()+"/"+res.at(1)+res.at(2));

            if (_saveWithOriginalFilename) {
                _IMAGE tmp;

                for (int i=0; i<images.count(); i++) {
                    if (images.at(i).largeURI.endsWith("/"+res.at(1)+res.at(2))) {
                        tmp = images.at(i);

                        f.setFileName(getSavepath()+"/"+tmp.originalFilename);
                        break;
                    }
                }
            }

            f.open(QIODevice::ReadWrite);
            bytesWritten = f.write(ba);
            f.close();

            if (bytesWritten == ba.size()) {
                setCompleted(requestURI, f.fileName());
            }
            else {
                QLOG_ERROR() << "UIImageOverview :: Couldn't save file from URI " << url.toString();
            }
        }

    }
    else {
        iParser->setURL(url);
        status = iParser->parseHTML(ba);

        if (status.hasErrors) {
            QLOG_ERROR() << "ImageOverview :: Parser error " << iParser->getErrorCode();
            switch (iParser->getErrorCode()) {
            case 404:
                stopDownload();
                processCloseRequest();
                break;

            default:
                break;
            }
        }
        else {
            if (status.isFrontpage) {
                QStringList newTab;
                QString v;

                v = getValues();
                newTab = v.split(";;");
                threadList = iParser->getUrlList();

                foreach (QUrl u, threadList) {
                    newTab.replace(0, u.toString());
                    emit createThreadRequest(newTab.join(";;"));
                }

                if (settings->value("options/close_overview_threads", true).toBool()) {
                    emit closeRequest(this, 0);
                }
            }
            else {
                if (status.hasImages) {
                    imageList = iParser->getImageList();
                    mergeImageList(imageList);
                }
            }
        }

        if (cached) {
            _rescan = false;
            timer->stop();
        }
    }
}

void ImageThread::setCompleted(QString uri, QString filename) {
    int i;
    _IMAGE tmp;

    for (i=0; i<images.length(); i++) {
        if (images.at(i).largeURI == uri) {
            tmp = images.at(i);
            tmp.downloaded = true;
            tmp.savedAs = filename;

            images.replace(i,tmp);

            if (isDownloadFinished()) {
                download(false);
            }

            break;
        }
    }
}

bool ImageThread::isImage(QUrl url) {
    bool ret;

    ret = false;

    if (url.toString().indexOf(QRegExp(__IMAGE_REGEXP__, Qt::CaseInsensitive)) != -1)
        ret = true;

    return ret;
}

void ImageThread::start(void) {
    QDir dir;
    QString parsedSavepath;

    _running = true;

    if (_sUrl != "") {

        if (!_sUrl.startsWith("http")) {
            _sUrl = _sUrl.prepend("http://");
        }

        // Check if we can parse this URI
        if (selectParser()) {
            parsedSavepath = getSavepath();
            if (parsedSavepath.endsWith("\\")) {
                parsedSavepath.chop(1);
            }
            QLOG_TRACE() << "ImageThread :: Setting save path to " << parsedSavepath;
            dir.setPath(parsedSavepath);

            if (!dir.exists()) {
                QDir d;

                d.mkpath(parsedSavepath);
                QLOG_INFO() << "ImageThread :: Directory" << parsedSavepath << " does not exist. Creating...";
            }

            if (dir.exists()) {
                startDownload();
                if (_rescan) {
                    timer->setInterval(_rescanInterval*1000);
                    timer->start();
                }
            }
            else
            {
                stop();
                emit errorMessage("Directory does not exist / Could not be created");
                QLOG_ERROR() << "ImageThread :: Directory" << parsedSavepath << " does not exist and I couldn't create it.";
            }
        }
        else {
            stop();
            emit errorMessage("Could not find a parser for this URL (" + _sUrl + ")");
            QLOG_WARN() << "ImageThread :: I couldn't find a parser for uri " << _sUrl;
        }
    }
}

void ImageThread::stop(void) {
    _running = false;
    stopDownload();
    timer->stop();

    if (oParser != 0) {
        oParser->deleteLater();
        oParser = 0;
        iParser = 0;
    }
}

bool ImageThread::checkForExistingThread(QString s) {
    return false;
}

void ImageThread::setBlackList(BlackList* bl) {
    _blackList = bl;
}

int ImageThread::getDownloadedImagesCount() {
    int ret;

    ret = 0;

    for (int i=0; i<images.count(); i++) {
        if (images.at(i).downloaded)
            ret++;
    }

    return ret;
}

QString ImageThread::getSavepath() {
    QString ret;

    if (iParser) {
        ret = iParser->parseSavepath(_sSavepath);
    }
    else {
        QLOG_WARN() << "ImageThread :: Called getSavepath() although no parser was loaded.";
        ret = _sSavepath;
    }

    return ret;
}
