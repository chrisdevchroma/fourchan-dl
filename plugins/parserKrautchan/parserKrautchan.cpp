#include "parserKrautchan.h"

ParserKrautchan::ParserKrautchan()
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

QString ParserKrautchan::getAuthor() {
  return "Josef Schneider";
}

QString ParserKrautchan::getPluginName() {
  return "krautchan.net Parser";
}

QString ParserKrautchan::getVersion() {
  return _LIB_VERSION;
}

QString ParserKrautchan::getDomain() {
  return "krautchan.net";
}

ParsingStatus ParserKrautchan::parseHTML(QString html) {
  QStringList res;
  QRegExp rxImageDiv("<div class=\"file_(?:reply|thread)\">(.*)</div>", Qt::CaseInsensitive, QRegExp::RegExp2);
  rxImageDiv.setMinimal(true);
  QRegExp rxImage("'/files/([^']+)'");
  QRegExp rxThumbnail("'/thumbnails/([^']+)'");
  QRegExp rxImageName("<span class=\"filename\"><a href=\"[^\"]+/([^/\"]+)\"[^>]*>");
  QRegExp rxThreads("<a href=\"([^\"]+)\"[^>]*>Antworten</a>", Qt::CaseInsensitive, QRegExp::RegExp2);
  QRegExp rxTitle("<span class=\"postsubject\">([^<]+)</span>");
  QRegExp rxThreadPage("<h2>Antwortmodus\\s+\\(Thread \\d+\\)</h2>");

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
  pageIsFrontpage = !html.contains(rxThreadPage);

  if (pageIsFrontpage) {
      pos = 0;
      _statusCode.isFrontpage = true;

      while (pos > -1) {
          pos = rxThreads.indexIn(html, pos + 1);
          res = rxThreads.capturedTexts();

          if (!res.at(1).isEmpty()) {
              sUrl = "http://krautchan.net"+res.at(1);
              if (sUrl.endsWith("/")) {
                  sUrl.remove(sUrl.length()-1,1);
                }

              _urlList.append(QUrl(sUrl));
            }
        }
    }
  else {
      // Checking for Images

      //extract all <div> containing a single image each
      QStringList images;
      pos = rxImageDiv.indexIn(html);
      while (pos > -1) {
          res = rxImageDiv.capturedTexts();
          images.append(res.at(1));
          pos = rxImageDiv.indexIn(html, pos+1);
        }
      //extract name, url and thumbnail url for each image
      QStringList::const_iterator image;
      for (image = images.constBegin(); image != images.constEnd();
           ++image) {
          i.originalFilename = i.thumbURI = i.largeURI = "";
          pos = rxImageName.indexIn(*image);
          if (pos > -1) {
              res = rxImageName.capturedTexts();
              i.originalFilename = res.at(1);
            }

//          pos = rxThumbnail.indexIn(*image);
//          if (pos > -1) {
//              res = rxThumbnail.capturedTexts();
//              i.thumbURI = "http://krautchan.net/thumbnails/"+res.at(1);
//            }

          pos = rxImage.indexIn(*image);
          if (pos > -1) {
              res = rxImage.capturedTexts();
              i.largeURI = "http://krautchan.net/files/"+ res.at(1);
              _images.append(i);
              _statusCode.hasImages = true;
            }
        }

      pos = 0;
      while (pos > -1) {
          pos = rxTitle.indexIn(html,pos+1);
          res = rxTitle.capturedTexts();

          if (pos > -1 && res.at(1) != "") {
              _threadTitle = res.at(1);
              _statusCode.hasTitle = true;
              pos = -1;
            }
        }
    }

  return _statusCode;
}

QString ParserKrautchan::getThreadTitle() {
  return _threadTitle;
}

QList<_IMAGE> ParserKrautchan::getImageList() {
  return _images;
}

QUrl ParserKrautchan::getRedirectURL() {
  return _redirect;
}

int ParserKrautchan::getErrorCode() {
  return _errorCode;
}

ParsingStatus ParserKrautchan::getStatusCode() {
  return _statusCode;
}

QList<QUrl> ParserKrautchan::getUrlList() {
  return _urlList;
}

void ParserKrautchan::setURL(QUrl url) {
  _url = url;

  boardName = _url.path().section("/",1,1);
  threadNumber = "";
  if (_url.path().contains("thread")) {
      QRegExp rxTid("/thread-(\\d+)\\.html");
      int pos = rxTid.indexIn(_url.path());
      QStringList res = rxTid.capturedTexts();

      if (pos > -1 && res.at(1) != "") {
          threadNumber = res.at(1);
        }
    }
}

QString ParserKrautchan::parseSavepath(QString s) {
  s.replace("%n", threadNumber);
  s.replace("%b", boardName);
  s.replace("%h", _url.host());

  return s;
}

QMap<QString, QString> ParserKrautchan::getSupportedReplaceCharacters() {
  QMap<QString, QString> ret;

  ret.insert("%n", "Threadnumber");
  ret.insert("%b", "Board");
  ret.insert("%h", "Host");

  return ret;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(pParserKrautchan, ParserKrautchan)
#endif
