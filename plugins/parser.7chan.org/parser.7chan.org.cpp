#include "parser.7chan.org.h"

Parser7ChanOrg::Parser7ChanOrg()
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

QString Parser7ChanOrg::getAuthor() {
    return "Mickey Fenton";
}

QString Parser7ChanOrg::getPluginName() {
    return "7chan.org Parser";
}

QString Parser7ChanOrg::getVersion() {
    return _LIB_VERSION;
}

QString Parser7ChanOrg::getDomain() {
    return "7chan.org";
}

ParsingStatus Parser7ChanOrg::parseHTML(QString html) {
    QStringList res;
    QRegExp rxImages("<a href=\"([^\"]+)\" id=\"expandimg_([^\"]+)\">[^<]+</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxThreads("<a href=\"([^\"]+)\"[^>]*>View</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"subject\">([^<]+)</span>");
    QRegExp rxEntireThread("<a href=\"([^\"]+)\">Entire Thread</a>");

    //bool imagesAdded;
    bool pageIsFrontpage;
    bool threadHasMorePages;
    int pos;
    _IMAGE i;
    QUrl u;
    QString sUrl;
    QString redirect;

    _html = html;
    _images.clear();
    _redirect.clear();
    _urlList.clear();
    _statusCode.hasErrors = false;
    _statusCode.hasImages = false;
    _statusCode.hasTitle = false;
    _statusCode.isFrontpage = false;
    _statusCode.hasRedirect = false;

    pos = 0;
    i.downloaded = false;
    i.requested = false;
//    pageIsFrontpage = !html.contains("<div id=\"ca_thread_html\">");
    pageIsFrontpage = html.count("</aside>") > 1 ? true:false;
    threadHasMorePages = html.count(">Entire Thread</a>") > 1 ? true:false;

    if (pageIsFrontpage) {
        pos = 0;
        _statusCode.isFrontpage = true;

        while (pos > -1) {
            pos = rxThreads.indexIn(html, pos + 1);
            res = rxThreads.capturedTexts();

            if (!res.at(1).isEmpty()) {
                sUrl = res.at(1);
                if (sUrl.endsWith("/")) {
                    sUrl.remove(sUrl.length()-1,1);
                }

                _urlList.append(QUrl(sUrl));
            }
        }
    }
    else if (threadHasMorePages) {
        rxEntireThread.indexIn(html, 1);
        res = rxEntireThread.capturedTexts();
        redirect = QString("http://www.7chan.org%1").arg(res.at(1));
        _redirect = redirect.replace("&amp;", "&");

        if (_redirect != _url.toString()) {
            _statusCode.hasRedirect = true;
        }
    }

    if (!pageIsFrontpage) {
        // Checking for Images
        pos = 0;

        while (pos > -1) {
            pos = rxImages.indexIn(html, pos+1);
            res = rxImages.capturedTexts();

            i.originalFilename = res.at(1).right(res.at(1).length() - res.at(1).lastIndexOf("/") - 1);
            i.largeURI = res.at(1);
            i.thumbURI = "";

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
                pos = -1;
            }
        }
    }

    return _statusCode;
}

QString Parser7ChanOrg::getThreadTitle() {
    return _threadTitle;
}

QList<_IMAGE> Parser7ChanOrg::getImageList() {
    return _images;
}

QUrl Parser7ChanOrg::getRedirectURL() {
    return _redirect;
}

int Parser7ChanOrg::getErrorCode() {
    return _errorCode;
}

ParsingStatus Parser7ChanOrg::getStatusCode() {
    return _statusCode;
}

QList<QUrl> Parser7ChanOrg::getUrlList() {
    return _urlList;
}

void Parser7ChanOrg::setURL(QUrl url) {
    _url = url;

    boardName = _url.path().section("/",2,2);
    _archiveName = _url.path().section("/",1,1);
//    if (_url.path().contains("res")) {
    threadNumber = _url.path().section("/",3,3);
//    }
//    else {
//        threadNumber = "";
//    }
}

QString Parser7ChanOrg::parseSavepath(QString s) {
//    s.replace("%n", threadNumber);
//    s.replace("%b", boardName);
    s.replace("%h", "7chan.org");
//    s.replace("%a", _archiveName);

    return s;
}

QMap<QString, QString> Parser7ChanOrg::getSupportedReplaceCharacters() {
    QMap<QString, QString> ret;

//    ret.insert("%n", "Threadnumber");
//    ret.insert("%b", "Board");
    ret.insert("%h", "Host (7chan.org)");
//    ret.insert("%a", "Archive (e. g. 4chan)");

    return ret;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParser7ChanOrg, Parser7ChanOrg)
#endif
