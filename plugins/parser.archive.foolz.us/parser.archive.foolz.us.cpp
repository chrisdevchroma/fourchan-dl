#include "parser.archive.foolz.us.h"

ParserArchiveFoolzUs::ParserArchiveFoolzUs()
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

QString ParserArchiveFoolzUs::getAuthor() {
    return "Mickey Fenton";
}

QString ParserArchiveFoolzUs::getPluginName() {
    return "archive.foolz.us Parser";
}

QString ParserArchiveFoolzUs::getVersion() {
    return _LIB_VERSION;
}

QString ParserArchiveFoolzUs::getDomain() {
    return "archive.foolz.us";
}

ParsingStatus ParserArchiveFoolzUs::parseHTML(QString html) {
    QStringList res;
    QRegExp rxImages("<div class=\"thread_image_box\"[^>]*>[^<]*<a href=\"([^\"]+)\"(?:[^<]+)(<[^>]*>)[^<]*</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxThreads("<a href=\"([^\"]+)\"[^>]*>View</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"subject\">([^<]+)</span>");

    //bool imagesAdded;
    bool pageIsFrontpage;
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
//    pageIsFrontpage = !html.contains("<div id=\"ca_thread_html\">");
    pageIsFrontpage = html.count("</aside>") > 1 ? true:false;

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
    else {
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

QString ParserArchiveFoolzUs::getThreadTitle() {
    return _threadTitle;
}

QList<_IMAGE> ParserArchiveFoolzUs::getImageList() {
    return _images;
}

QUrl ParserArchiveFoolzUs::getRedirectURL() {
    return _redirect;
}

int ParserArchiveFoolzUs::getErrorCode() {
    return _errorCode;
}

ParsingStatus ParserArchiveFoolzUs::getStatusCode() {
    return _statusCode;
}

QList<QUrl> ParserArchiveFoolzUs::getUrlList() {
    return _urlList;
}

void ParserArchiveFoolzUs::setURL(QUrl url) {
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

QString ParserArchiveFoolzUs::parseSavepath(QString s) {
//    s.replace("%n", threadNumber);
//    s.replace("%b", boardName);
    s.replace("%h", "archive.foolz.us");
//    s.replace("%a", _archiveName);

    return s;
}

QMap<QString, QString> ParserArchiveFoolzUs::getSupportedReplaceCharacters() {
    QMap<QString, QString> ret;

//    ret.insert("%n", "Threadnumber");
//    ret.insert("%b", "Board");
    ret.insert("%h", "Host (archive.foolz.us)");
//    ret.insert("%a", "Archive (e. g. 4chan)");

    return ret;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParserArchiveFoolzUs, ParserArchiveFoolzUs)
#endif
