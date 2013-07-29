#include "parserchanarchive.h"

ParserChanArchive::ParserChanArchive()
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

QString ParserChanArchive::getAuthor() {
    return "Mickey Fenton";
}

QString ParserChanArchive::getPluginName() {
    return "chanarchive.org Parser";
}

QString ParserChanArchive::getVersion() {
    return _LIB_VERSION;
}

QString ParserChanArchive::getDomain() {
    return "chanarchive.org";
}

ParsingStatus ParserChanArchive::parseHTML(QString html) {
    QStringList res;
    QList<QRegExp> imageRegExps;
    QList<RegExpPosition> imageRegExpsPos;
    RegExpPosition rep;

    rep.filename = -1;
    rep.imagelink = -1;

    imageRegExps << QRegExp("<span class=\"fileText\"[^>]*>[^<]*<a href=\"([^/]*)/([^\"]+)\"(?:[^<]+)</a>[^<]*<span title=\"([^\"]+)\">[^<]+</span>", Qt::CaseInsensitive, QRegExp::RegExp2);
    rep.filename  = 3;
    rep.imagelink = 2;
    imageRegExpsPos << rep;

    imageRegExps << QRegExp("<span class=\"filesize\"[^>]*>[^<]*<a href=\"([^/]*)/([^\"]+)\"(?:[^<]+)</a>[^<]*<span title=\"([^\"]+)\">[^<]+</span>", Qt::CaseInsensitive, QRegExp::RegExp2);
    imageRegExpsPos << rep;

    imageRegExps << QRegExp("<span title=\"([^\"]+)\">[^>]+</span>\\)</span><br><a href=\"([^/]*)/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
    imageRegExpsPos << rep;


    QRegExp rxThreads("<div id=\"ca_ctl_title\">[^<]*<a href=\"([^\"]+)\">([^<]+)</a>", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitle("<span class=\"subject\">([^<]+)</span>");

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
    pageIsFrontpage = !html.contains("<div id=\"ca_thread_html\">");

    if (pageIsFrontpage) {
        pos = 0;
        _statusCode.isFrontpage = true;

        while (pos > -1) {
            pos = rxThreads.indexIn(html, pos + 1);
            res = rxThreads.capturedTexts();

            if (!res.at(1).isEmpty()) {
                sUrl = QString("%1://%2%3").arg(_url.scheme()).arg(_url.host()).arg(res.at(1));

                _urlList.append(QUrl(sUrl));
            }
        }
    }
    else {
        // Checking for Images
        pos = 0;

        if (imageRegExps.count() > 0) {
            QRegExp rx;

            for (int k=0; k<imageRegExps.count(); k++){
                rx = imageRegExps.value(k);
                rep = imageRegExpsPos.value(k);
                pos = 0;

                while ( pos > -1 ) {
                    pos = rx.indexIn(html, pos+1);
                    res = rx.capturedTexts();
                    if (rep.filename != -1) {
                        i.originalFilename = res.at(rep.filename);
                    }
                    if (rep.imagelink != -1) {
                        i.largeURI = QString("%1://%2/%3").arg(_url.scheme()).arg(_url.host()).arg(res.at(rep.imagelink));
                    }

                    if (pos != -1) {
                        _images.append(i);
                        _statusCode.hasImages = true;
                    }
                }
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

QString ParserChanArchive::getThreadTitle() {
    return _threadTitle;
}

QList<_IMAGE> ParserChanArchive::getImageList() {
    return _images;
}

QUrl ParserChanArchive::getRedirectURL() {
    return _redirect;
}

int ParserChanArchive::getErrorCode() {
    return _errorCode;
}

ParsingStatus ParserChanArchive::getStatusCode() {
    return _statusCode;
}

QList<QUrl> ParserChanArchive::getUrlList() {
    return _urlList;
}

void ParserChanArchive::setURL(QUrl url) {
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

QString ParserChanArchive::parseSavepath(QString s) {
    s.replace("%n", threadNumber);
    s.replace("%b", boardName);
    s.replace("%h", "chanarchive.org");
    s.replace("%a", _archiveName);

    return s;
}

QMap<QString, QString> ParserChanArchive::getSupportedReplaceCharacters() {
    QMap<QString, QString> ret;

    ret.insert("%n", "Threadnumber");
    ret.insert("%b", "Board");
    ret.insert("%h", "Host (chanarchive.org)");
    ret.insert("%a", "Archive (e. g. 4chan)");

    return ret;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParserChanArchive, ParserChanArchive)
#endif
