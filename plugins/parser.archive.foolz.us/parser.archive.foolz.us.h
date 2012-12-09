#ifndef PARSERARCHIVEFOOLZUS_H
#define PARSERARCHIVEFOOLZUS_H

#include <QStringList>
#include <QList>
#include <QRegExp>
#include <QtDebug>
#include "../../gui/ParserPluginInterface.h"
#include "../../gui/structs.h"

class ParserArchiveFoolzUs : public QObject, public ParserPluginInterface {
    Q_OBJECT
    Q_INTERFACES(ParserPluginInterface)

public:
    ParserArchiveFoolzUs();
    QString getAuthor(void);
    QString getPluginName(void);
    QString getVersion();
    QString getDomain();
    QString getInterfaceRevision() {return _PARSER_PLUGIN_INTERFACE_REVISION;}
    ParsingStatus parseHTML(QString html);
    void setURL(QUrl url);
    QString getThreadTitle();
    QList<_IMAGE> getImageList();
    QList<QUrl> getUrlList();
    QUrl getRedirectURL();
    int getErrorCode();
    ParsingStatus getStatusCode();
    QString parseSavepath(QString s);
    QMap<QString, QString> getSupportedReplaceCharacters();

    QObject* createInstance() {return new ParserArchiveFoolzUs();}

private:
    QString _html;
    QString boardName;
    QString threadNumber;
    QList<_IMAGE> _images;
    QList<QUrl> _urlList;
    QUrl _redirect;
    QUrl _url;
    ParsingStatus _statusCode;
    int _errorCode;
    QString _threadTitle;
    QString _archiveName;
};

#endif // PARSERARCHIVEFOOLZUS_H