#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
}

void PluginManager::loadPlugins(void)
{
    QDir pluginDir(QApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginDir.dirName().toLower() == "debug"
            || pluginDir.dirName().toLower() == "release")
        pluginDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginDir.dirName() == "MacOS") {
        pluginDir.cdUp();
        pluginDir.cdUp();
        pluginDir.cdUp();
    }
#endif
    if (!pluginDir.cd("plugins")) {
        qDebug() << "Plugin directory not found: " << pluginDir.path();
    }
    else {
        qDebug() << "Plugin Search path: " << pluginDir.path();
        foreach (QString fileName, pluginDir.entryList(QDir::Files))
        {
            qDebug() << "Checking file " << pluginDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
            qDebug() << loader.errorString();
            QString str;
            qDebug() << "Intances : " << loader.staticInstances();
            if (ParserPluginInterface* interface =
                    qobject_cast<ParserPluginInterface *>(loader.instance()))
            {
                str.append(interface->getPluginName());
//                str.append(";;;");
//                str.append(pluginDir.absoluteFilePath(fileName));
                pluginList.append(str);
                loadedPlugins.append(interface);
            }
            else {
                qDebug() << "error loading lib" << loader.errorString();
            }
        }
        qDebug() << "PluginList" << pluginList;
    }
}

ParserPluginInterface* PluginManager::getParser(QUrl url, bool* ok) {
    QString domain;
    ParserPluginInterface* ret;

    domain = url.host();
    ret = 0;
    *ok = false;

    for (int i=0; i<loadedPlugins.count(); i++) {
        if (domain.indexOf(loadedPlugins.at(i)->getDomain()) != -1) {
                ret = loadedPlugins.at(i);
                *ok = true;

                break;
        }
    }

    return ret;
}

ParserPluginInterface* PluginManager::getPlugin(int i) {
    ParserPluginInterface* ret;

    ret = 0;

    if (i<loadedPlugins.count()) {
        ret = loadedPlugins.at(i);
    }

    return ret;
}

QStringList PluginManager::getAvailablePlugins() {
    return pluginList;
}
