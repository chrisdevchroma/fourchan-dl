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
    QStringList getAvailablePlugins(void);
    component_information getInfo(QString name);
    ParserPluginInterface* getParser(QUrl, bool*);
    ParserPluginInterface* getPlugin(int);
    ParserPluginInterface* getPlugin(QString);
private:
    QList<ParserPluginInterface*> loadedPlugins;
    QStringList pluginList;
    QMap<QString, component_information> components;
    void loadPlugins(void);
signals:

public slots:

};

#endif // PLUGINMANAGER_H
