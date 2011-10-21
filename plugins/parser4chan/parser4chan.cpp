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
    return "Florian Born";
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
    QRegExp rx("<span title=\"([^\"]+)\">[^>]+</span>\\)</span><br><a href=\"http://images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp boardPage("<a href=\"res/(\\d+)\">Reply</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"filetitle\">([^<]+)</span>");
    bool imagesAdded;
    bool pageIsFrontpage;
    int pos;
    _IMAGE i;
    QUrl u;
    QString sUrl;
    QString boardName;

    _html = html;
    _images.clear();
    _redirect.clear();
    _urlList.clear();
    _statusCode.hasErrors = false;
    _statusCode.hasImages = false;
    _statusCode.hasTitle = false;
    _statusCode.isFrontpage = false;

    imagesAdded = false;
    pos = 0;
    i.downloaded = false;
    i.requested = false;
    pageIsFrontpage = false;

    if (html.contains("<title>4chan - Banned</title>")) {
        _statusCode.hasErrors = true;
        _errorCode = 999;
    }
    else {
        // CHecking for Thread Links
        while (pos > -1) {
            pos = boardPage.indexIn(html, pos+1);
            res = boardPage.capturedTexts();

            if (res.at(1) != "") {
                pageIsFrontpage=true;
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
            pos = rx.indexIn(html, pos+1);
            res = rx.capturedTexts();
            QUrl temp = QUrl::fromEncoded(res.at(1).toAscii());

            i.originalFilename = temp.toString();
            i.largeURI = "http://images.4chan.org/"+res.at(2);
            i.thumbURI = res.at(3);

            if (pos != -1) {
                _images.append(i);
                _statusCode.hasImages = true;
            }
        }

        pos = 0;
        while (pos > -1) {
            pos = rxTitle.indexIn(html,pos+1);
            res = rxTitle.capturedTexts();

            if (res.at(1) != "") {
                _threadTitle = res.at(1);
                _statusCode.hasTitle = true;
            }

        }
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
}

Q_EXPORT_PLUGIN2(pParser4chan, Parser4chan);
