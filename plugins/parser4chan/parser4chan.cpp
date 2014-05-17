#include "parser4chan.h"

Parser4chan::Parser4chan()
{
    _html = "";
    _statusCode.hasErrors = false;
    _statusCode.hasImages = false;
    _statusCode.hasTitle = false;
    _statusCode.isFrontpage = false;
    _statusCode.hasRedirect = false;
    _statusCode.threadFragmented = false;
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
    //QRegExp rxImagesNew("<span class=\"fileText\"[^>]*>[^<]*<a href=\"([^/]*)//i\\.4cdn\\.org/([^\"]+)\"(?:[^<]+)</a>[^<]*(<span title=\"([^\"]+)\">[^<]+)*</span>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxImagesNew("<div class=\"fileText\"[^>]*>[^<]*<a href=\"([^/]*)//([^/]+)/([^\"]+)\"(?:[^<]+)</a>[^<]*<span[^>]*>([^<]+)*</span>", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxThreadsNew("<div class=\"thread\" id=\"t([^\"]+)\">", Qt::CaseSensitive, QRegExp::RegExp2);
    QRegExp rxTitleNew("<span class=\"subject\">([^<]+)</span>");
//    QRegExp rxImagesOld("<span title=\"([^\"]+)\">[^>]+</span>\\)</span><br><a href=\"([^/]*)//images\\.4chan\\.org/([^\"]+)\"(?:[^<]+)<img src=([^\\s]+)(?:[^<]+)</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
//    QRegExp rxThreadsOld("<a href=\"res/(\\d+)\">Reply</a>", Qt::CaseSensitive, QRegExp::RegExp2);
//    QRegExp rxTitleOld("<span class=\"filetitle\">([^<]+)</span>");

    //bool imagesAdded;
    //bool pageIsFrontpage;
    int pos;
    _IMAGE img;
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
    img.downloaded = false;
    img.requested = false;

    if (html.startsWith("{\"posts\":")) {   // JSON Thread
        QStringList posts;
        QMap<QString,QString> postDetails;
        QString key, value;
        int i,k;
        bool first_post;

        html.replace("{\"posts\":[{", "");   // Remove unnecessary header
        html.replace("}]}", "");             //  -"- footer

        posts = html.split("},{");
//        qDebug() << "Found " << posts.count() << "posts\n";
        first_post = true;
        foreach (QString post, posts) {
//            qDebug() << "starting parsing post: " + post +"\n";

            if (post.contains(",\"tim\":")) {
                postDetails.clear();
                i = k = 0;

                bool stop = false;
                while (!stop) {
                    k = post.indexOf(":", i);
//                    qDebug() << "k:" << k;
                    if (k == -1) {
                        stop = true;
                        break;
                    }
                    key = post.mid(i,k-i);
                    i = k+1;
                    k = post.indexOf(",\"", i);
                    if (k == -1) {
                        stop=true;
                        value = post.right(post.length()-i);
                    }
                    else {
                        value = post.mid(i, k-i);
                        i = k+1;
                    }

                    if (key.startsWith("\""))   key.remove(0,1);
                    if (value.startsWith("\"")) value.remove(0,1);
                    if (key.endsWith("\""))   key.remove(key.length()-1,1);
                    if (value.endsWith("\"")) value.remove(value.length()-1,1);

                    postDetails.insert(key, value);
                }

                if (postDetails.contains("tim")) { // Image in post
                    img.largeURI = QString("http://i.4cdn.org/%3/%1%2")
                            .arg(postDetails.value("tim"))
                            .arg(postDetails.value("ext"))
                            .arg(boardName);
                    img.originalFilename = QString("%1%2")
                            .arg(postDetails.value("filename"))
                            .arg(postDetails.value("ext"));

                    _images.append(img);
                    _statusCode.hasImages = true;

//                    qDebug() << "Found image " << img.largeURI << " (" << img.originalFilename << ")\n";
                }
                if (postDetails.contains("sub") && !_statusCode.hasTitle) {
                    _statusCode.hasTitle = true;
                    _threadTitle = postDetails.value("sub");
                }
                else if (first_post && postDetails.contains("com")) {
                    _statusCode.hasTitle = true;
                    _threadTitle = postDetails.value("com");

                    if (_threadTitle.length() > 50) {
                        _threadTitle = postDetails.value("com").left(postDetails.value("com").indexOf(" ", 49));
                    }
                }
            }
            first_post = false;
        }

    }
    else if (html.startsWith("{\"threads\":")) {    //JSON threadlist
        int i,k;
        bool stop;

        html.replace("{\"threads\":[{", "");   // Remove unnecessary header
        html.replace("}]}", "");             //  -"- footer

        stop = false;

        i = k = 0;
        while (!stop) {

            i = html.indexOf("\"posts\":[{\"no\":", i);
            if (i == -1) {
                stop = true;
                break;
            }
            i+=15;
            k = html.indexOf(",", i);

            if (k == -1) {
                stop = true;
                break;
            }
            sUrl = QString("%1://boards.4chan.org/%2/thread/%3").arg(_url.scheme()).arg(boardName).arg(html.mid(i, k-i));
            _urlList << QUrl(sUrl);
            _statusCode.isFrontpage = true;
            i=k+1;
        }
    }
    else {
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
                            u.setUrl(QString("thread/%1").arg(res.at(1)));

                            // build complete url

                            if (u.isRelative()) {
                                sUrl = "";
                                if (u.path().startsWith("/")) {
                                    // We need to complete only the host
                                    sUrl = QString("%1/%2").arg(_url.host()).arg(u.path());
                                }
                                else if (u.path().startsWith("res") || u.path().startsWith("thread")) {
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
                            img.originalFilename = res.at(2).right(res.at(2).length() - res.at(2).lastIndexOf("/") - 1);
    //                        img.originalFilename = res.at(3).right(res.at(3).length() - res.at(3).lastIndexOf("/") - 1);
                        }
                        else {
    //                        img.originalFilename = res.at(5);
                            img.originalFilename = res.at(4);
                        }
                        img.largeURI = "http://"+res.at(2)+"/"+res.at(3);
                        img.thumbURI = "";

                        if (pos != -1) {
                            _images.append(img);
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
            else {  // 'Old' Thread layout not supported anymore for maintance reasons
                _statusCode.hasErrors = true;
                _errorCode = 1;
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
    QString s;
    QString s_url;

    s_url = url.toString();

    if (s_url.endsWith(".json")) {
        s = url.toString();
        s.replace(".json", "");
        _url = QUrl(s);
    }
    else {
        _url = url;
    }

    boardName = _url.path().section("/",1,1);
    if (_url.path().contains("res") || _url.path().contains("thread/")) {
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
    s.replace("%a", "");

    s.replace("//", "/");

    return s;
}

QMap<QString, QString> Parser4chan::getSupportedReplaceCharacters() {
    QMap<QString, QString> ret;

    ret.insert("%n", "Threadnumber");
    ret.insert("%b", "Board");
    ret.insert("%h", "Host");

    return ret;
}

void Parser4chan::initPlugin() {

}

QList<QUrl> Parser4chan::initialRequests() {
    QList<QUrl> uris;

//    uris.append(QUrl("http://boards.4chan.org/i/#cfg=%7B%22settings%22%3A%22%7B%5C%22quotePreview%5C%22%3Atrue%2C%5C%22backlinks%5C%22%3Atrue%2C%5C%22quickReply%5C%22%3Atrue%2C%5C%22threadUpdater%5C%22%3Afalse%2C%5C%22threadHiding%5C%22%3Afalse%2C%5C%22pageTitle%5C%22%3Afalse%2C%5C%22hideGlobalMsg%5C%22%3Atrue%2C%5C%22alwaysAutoUpdate%5C%22%3Afalse%2C%5C%22topPageNav%5C%22%3Afalse%2C%5C%22threadWatcher%5C%22%3Afalse%2C%5C%22imageExpansion%5C%22%3Afalse%2C%5C%22fitToScreenExpansion%5C%22%3Afalse%2C%5C%22threadExpansion%5C%22%3Afalse%2C%5C%22alwaysDepage%5C%22%3Afalse%2C%5C%22imageSearch%5C%22%3Afalse%2C%5C%22reportButton%5C%22%3Atrue%2C%5C%22localTime%5C%22%3Atrue%2C%5C%22stickyNav%5C%22%3Atrue%2C%5C%22keyBinds%5C%22%3Atrue%2C%5C%22inlineQuotes%5C%22%3Atrue%2C%5C%22filter%5C%22%3Afalse%2C%5C%22revealSpoilers%5C%22%3Atrue%2C%5C%22replyHiding%5C%22%3Afalse%2C%5C%22imageHover%5C%22%3Afalse%2C%5C%22threadStats%5C%22%3Afalse%2C%5C%22IDColor%5C%22%3Atrue%2C%5C%22downloadFile%5C%22%3Atrue%2C%5C%22inlineReport%5C%22%3Atrue%2C%5C%22noPictures%5C%22%3Afalse%2C%5C%22embedYouTube%5C%22%3Afalse%2C%5C%22embedSoundCloud%5C%22%3Afalse%2C%5C%22updaterSound%5C%22%3Afalse%2C%5C%22customCSS%5C%22%3Afalse%2C%5C%22autoScroll%5C%22%3Afalse%2C%5C%22hideStubs%5C%22%3Afalse%2C%5C%22compactThreads%5C%22%3Afalse%2C%5C%22dropDownNav%5C%22%3Afalse%2C%5C%22fixedThreadWatcher%5C%22%3Afalse%2C%5C%22persistentQR%5C%22%3Afalse%2C%5C%22disableAll%5C%22%3Afalse%2C%5C%22TW-position%5C%22%3A%5C%22left%3A%2019px%3B%20top%3A%20387px%3B%5C%22%2C%5C%22customMenu%5C%22%3Afalse%2C%5C%22embedVocaroo%5C%22%3Afalse%7D%22%7D"));

    return uris;
}

QUrl Parser4chan::alterUrl(QUrl u){
    // Function is only called for non-images!
    // New URI format is http[s]://[..].4chan.org/<board>/thread/<threadnumber>[/threadtitle]
    //  Adding .json only works when the threadtitle is ommitted from the uri
    QString s_url;
    QStringList sl;

    s_url = u.toString();
    s_url.replace("/res/", "/thread/");

    if (!s_url.startsWith("http")) {
        s_url.prepend("http://");
    }

    if (s_url.endsWith("/")) s_url.remove(s_url.length()-1,1);

    sl = s_url.split("/");
    if (sl.count() == 4) {
        // This is the first page of a board.
        //  To get the .json append working we have to add "/1"
        s_url.append("/1");
    }
    if (sl.contains("thread")) {
        if (sl.count() == 7) {
            sl.removeLast();
            s_url = sl.join("/");
        }
    }
    if (!s_url.endsWith(".json")) s_url.append(".json");   // Only get the JSON files

    return QUrl(s_url);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParser4chan, Parser4chan)
#endif
