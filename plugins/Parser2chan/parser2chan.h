#ifndef Parser2chan_H
#define Parser2chan_H

//#include "parser2chan_global.h"
#include <QStringList>
#include <QList>
#include <QRegExp>
#include "../../ParserPluginInterface.h"
#include "../../structs.h"

class Parser2chan : public QObject, public ParserPluginInterface {
    Q_OBJECT
    Q_INTERFACES(ParserPluginInterface)

public:
    Parser2chan();
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

    QObject* createInstance() {return new Parser2chan();}

private:
    QString _html;
    QList<_IMAGE> _images;
    QList<QUrl> _urlList;
    QUrl _redirect;
    ParsingStatus _statusCode;
    int _errorCode;
    QString _threadTitle;
};

#endif // Parser2chan_H
