#ifndef PARSERPLUGININTERFACE_H
#define PARSERPLUGININTERFACE_H
#include <QtPlugin>
#include <QUrl>
#include "structs.h"

#define _PARSER_PLUGIN_INTERFACE_REVISION "1.4"

class ParserPluginInterface //: public QObject
{
public:
    virtual ~ParserPluginInterface() { }
    virtual QString getAuthor(void) = 0;
    virtual QString getPluginName(void) = 0;
    virtual QString getVersion() = 0;
    virtual QString getDomain() = 0;
    virtual QString getInterfaceRevision() = 0;
    virtual QString parseSavepath(QString s) = 0;
    virtual QMap<QString, QString> getSupportedReplaceCharacters() = 0;

    virtual ParsingStatus parseHTML(QString html) = 0;
    virtual void setURL(QUrl url) = 0;
    virtual QList<_IMAGE> getImageList() = 0;
    virtual QList<QUrl> getUrlList() = 0;
    virtual QUrl getRedirectURL() = 0;
    virtual int getErrorCode() = 0;
    virtual ParsingStatus getStatusCode() = 0;
    virtual QString getThreadTitle() = 0;

    virtual QObject* createInstance(void) = 0;

signals:


};
Q_DECLARE_INTERFACE(ParserPluginInterface,
                    "de.fourchan-dl.ParserPluginInterface/1.0")

#endif // PARSERPLUGININTERFACE_H
