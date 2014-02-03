#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QtDebug>
#include <QPluginLoader>
#include <QDir>
#include "QsLog.h"
#include "ParserPluginInterface.h"
#include "downloadmanager.h"

extern DownloadManager* downloadManager;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = 0);
    QStringList getAvailablePlugins(void);
    component_information getInfo(QString name);
    ParserPluginInterface* getParser(QUrl, bool*);
    ParserPluginInterface* getPlugin(int);
    ParserPluginInterface* getPlugin(QString);
    QStringList getSupportedDomains();
    bool isSupported(QString);
private:
    QList<ParserPluginInterface*> loadedPlugins;
    QStringList pluginList;
    QMap<QString, component_information> components;
    void loadPlugins(void);
    RequestHandler* requestHandler;
signals:

public slots:

};

#endif // PLUGINMANAGER_H
