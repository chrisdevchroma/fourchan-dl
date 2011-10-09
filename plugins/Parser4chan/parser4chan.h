#ifndef PARSER4CHAN_H
#define PARSER4CHAN_H

//#include "4chanParser_global.h"
#include <QStringList>
#include <QList>
#include <QRegExp>
#include "../../ParserPluginInterface.h"
#include "../../structs.h"

class Parser4chan : public QObject, public ParserPluginInterface {
    Q_OBJECT
    Q_INTERFACES(ParserPluginInterface)

public:
    Parser4chan();
    QString getAuthor(void);
    QString getPluginName(void);
    QString getVersion();
    QString getDomain();
    QString getInterfaceRevision() {return _PARSER_PLUGIN_INTERFACE_REVISION;}
    ParsingStatus parseHTML(QString html);
    QString getThreadTitle();
    QList<_IMAGE> getImageList();
    QList<QUrl> getUrlList();
    QUrl getRedirectURL();
    int getErrorCode();
    ParsingStatus getStatusCode();

    QObject* createInstance() {return new Parser4chan();}

private:
    QString _html;
    QList<_IMAGE> _images;
    QList<QUrl> _urlList;
    QUrl _redirect;
    ParsingStatus _statusCode;
    int _errorCode;
    QString _threadTitle;
};

#endif // PARSER4CHAN_H
