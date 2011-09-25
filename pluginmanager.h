#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QApplication>
#include <QtDebug>
#include <QPluginLoader>
#include <QDir>
#include "ParserPluginInterface.h"

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = 0);
    void loadPlugins(void);
    QStringList getAvailablePlugins(void);
    ParserPluginInterface* getParser(QUrl, bool*);
    ParserPluginInterface* getPlugin(int);
private:
    QList<ParserPluginInterface*> loadedPlugins;
    QStringList pluginList;
signals:

public slots:

};

#endif // PLUGINMANAGER_H
