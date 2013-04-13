#include "parser4chan.h"

Parser4chan::Parser4chan()
{
    _html = "";
    _statusCode.hasErrors = false;
    _statusCode.hasImages = false;
    _statusCode.hasTitle = false;
    _statusCode.isFrontpage = false;
    _errorCode = 0;
    _redirect = QUrl();
    _images.clear();
}

QString Parser4chan::getAuthor() {
    return "Mickey Fenton";
}

QString Parser4chan::getPluginName() {
    return "4chan.org Parser";
}

QString Parser4chan::getVersion() {
    return _LIB_VERSION;
}

QString Parser4chan::getDomain() {
    return "4chan.org";
}

ParsingStatus Parser4chan::parseHTML(QString html) {
    QStringList res;
    QRegExp rxImagesNew("<span class=\"fileText\"[^>]*>[^<]*<a href=\"([^/]*)//images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)</a>[^<]*(<span title=\"([^\"]+)\">[^<]+)*</span>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxThreadsNew("<div class=\"thread\" id=\"t([^\"]+)\">", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitleNew("<span class=\"subject\">([^<]+)</span>");
    QRegExp rxImagesOld("<span title=\"([^\"]+)\">[^>]+</span>\\)</span><br><a href=\"([^/]*)//images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxThreadsOld("<a href=\"res/(\\d+)\">Reply</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitleOld("<span class=\"filetitle\">([^<]+)</span>");

    //bool imagesAdded;
    //bool pageIsFrontpage;
    int pos;
    _IMAGE i;
    QUrl u;
    QString sUrl;

    _html = html;
    _images.clear();
    _redirect.clear();
    _urlList.clear();
    _statusCode.hasErrors = false;
    _statusCode.hasImages = false;
    _statusCode.hasTitle = false;
    _statusCode.isFrontpage = false;

    pos = 0;
    i.downloaded = false;
    i.requested = false;

    if (html.contains("<title>4chan - Banned</title>")) {
        _statusCode.hasErrors = true;
        _errorCode = 999;
    }
    else {
        if (html.contains("<div class=\"board\">")) {
            // Page is using the new html
            // Check if this is a thread overview
            if (html.count("<div class=\"thread\"") > 1) {
                while (pos > -1) {
                    pos = rxThreadsNew.indexIn(html, pos+1);
                    res = rxThreadsNew.capturedTexts();

                    if (res.at(1) != "") {
                        u.setUrl(QString("res/%1").arg(res.at(1)));

                        // build complete url

                        if (u.isRelative()) {
                            sUrl = "";
                            if (u.path().startsWith("/")) {
                                // We need to complete only the host
                                sUrl = QString("%1/%2").arg(_url.host()).arg(u.path());
                            }
                            else if (u.path().startsWith("res")) {
                                sUrl = QString("%1://boards.4chan.org/%2/%3").arg(_url.scheme()).arg(boardName).arg(u.path());
                            }
                            else {
                                qDebug() << "Parsing front page and don't know what to do. Found url" << u.toString();
                            }
                        }

                        _urlList << QUrl(sUrl);
                        _statusCode.isFrontpage = true;
                        _threadTitle = _url.toString();
                        _statusCode.hasTitle = true;
                    }
                }
            }
            else {
                // Checking for Images
                pos = 0;

                while (pos > -1) {
                    pos = rxImagesNew.indexIn(html, pos+1);
                    res = rxImagesNew.capturedTexts();

                    if (res.at(4).isEmpty()) {
                        i.originalFilename = res.at(2).right(res.at(2).length() - res.at(2).lastIndexOf("/") - 1);
                    }
                    else {
                        i.originalFilename = res.at(4);
                    }
                    i.largeURI = "http://images.4chan.org/"+res.at(2);
                    i.thumbURI = "";

                    if (pos != -1) {
                        _images.append(i);
                        _statusCode.hasImages = true;
                    }
                }

                pos = 0;
                while (pos > -1) {
                    pos = rxTitleNew.indexIn(html,pos+1);
                    res = rxTitleNew.capturedTexts();

                    if (res.at(1) != "") {
                        _threadTitle = res.at(1);
                        _statusCode.hasTitle = true;
                        pos = -1;
                    }
                }
            }
        }
        else {
            //Page is using the old html

            // CHecking for Thread Links
            while (pos > -1) {
                pos = rxThreadsOld.indexIn(html, pos+1);
                res = rxThreadsOld.capturedTexts();

                if (res.at(1) != "") {
                    u.setUrl(QString("res/%1").arg(res.at(1)));

                    // build complete url
                    boardName = _url.path().section("/", 1, 1);

                    if (u.isRelative()) {
                        sUrl = "";
                        if (u.path().startsWith("/")) {
                            // We need to complete only the host
                            sUrl = QString("%1/%2").arg(_url.host()).arg(u.path());
                        }
                        else if (u.path().startsWith("res")) {
                            sUrl = QString("http://boards.4chan.org/%1/%2").arg(boardName).arg(u.path());
                        }
                        else {
                            qDebug() << "Parsing front page and don't know what to do. Found url" << u.toString();
                        }
                    }

                    _urlList << QUrl(sUrl);
                    _statusCode.isFrontpage = true;
                }
            }

            // Checking for Images
            pos = 0;
            while (pos > -1) {
                pos = rxImagesOld.indexIn(html, pos+1);
                res = rxImagesOld.capturedTexts();
                QUrl temp = QUrl::fromEncoded(res.at(1).toLatin1());

                i.originalFilename = temp.toString();
                i.largeURI = "http://images.4chan.org/"+res.at(3);
                i.thumbURI = res.at(4);

                if (pos != -1) {
                    _images.append(i);
                    _statusCode.hasImages = true;
                }
            }

            pos = 0;
            while (pos > -1) {
                pos = rxTitleOld.indexIn(html,pos+1);
                res = rxTitleOld.capturedTexts();

                if (res.at(1) != "") {
                    _threadTitle = res.at(1);
                    _statusCode.hasTitle = true;
                }

            }
        }
    }

    if (_threadTitle.trimmed().isEmpty() && !_statusCode.isFrontpage) {
        _threadTitle = QString("Thread# %1").arg(threadNumber);
        _statusCode.hasTitle = true;
    }

    return _statusCode;
}

QString Parser4chan::getThreadTitle() {
    return _threadTitle;
}

QList<_IMAGE> Parser4chan::getImageList() {
    return _images;
}

QUrl Parser4chan::getRedirectURL() {
    return _redirect;
}

int Parser4chan::getErrorCode() {
    return _errorCode;
}

ParsingStatus Parser4chan::getStatusCode() {
    return _statusCode;
}

QList<QUrl> Parser4chan::getUrlList() {
    return _urlList;
}

void Parser4chan::setURL(QUrl url) {
    _url = url;

    boardName = _url.path().section("/",1,1);
    if (_url.path().contains("res")) {
        threadNumber = _url.path().section("/",3,3);
    }
    else {
        threadNumber = "";
    }
}

QString Parser4chan::parseSavepath(QString s) {
    s.replace("%n", threadNumber);
    s.replace("%b", boardName);
    s.replace("%h", _url.host());

    return s;
}

QMap<QString, QString> Parser4chan::getSupportedReplaceCharacters() {
    QMap<QString, QString> ret;

    ret.insert("%n", "Threadnumber");
    ret.insert("%b", "Board");
    ret.insert("%h", "Host");

    return ret;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParser4chan, Parser4chan)
#endif
