#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    loadPlugins();
}

void PluginManager::loadPlugins(void)
{
    QDir pluginDir(QApplication::applicationDirPath());
    component_information c;
#ifdef __DEBUG__
#if defined(Q_OS_WIN)
//        pluginDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginDir.dirName() == "MacOS") {
        pluginDir.cdUp();
        pluginDir.cdUp();
        pluginDir.cdUp();
    }
#endif
#endif
    if (!pluginDir.cd("plugins")) {
        QLOG_WARN() << "PluginManager :: " << "Plugin directory not found: " << pluginDir.path();
    }
    else {
        QLOG_INFO() << "PluginManager :: " << "Plugin Search path: " << pluginDir.path();

        foreach (QString fileName, pluginDir.entryList(QDir::Files))
        {
            QLOG_INFO() << "PluginManager :: " << "Checking file " << pluginDir.absoluteFilePath(fileName);
            QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
//            QLOG_ERROR() << loader.errorString();
            QString str;
            if (ParserPluginInterface* interface =
                    qobject_cast<ParserPluginInterface *>(loader.instance()))
            {
                if (interface->getInterfaceRevision() == _PARSER_PLUGIN_INTERFACE_REVISION) {
                    c.componentName = interface->getPluginName();
                    c.filename = fileName;
                    c.type = "plugin/parser";
                    c.version = interface->getVersion();

                    components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
                    str.append(interface->getPluginName());
                    str.append(";;;");
                    str.append(fileName);
                    pluginList.append(str);
                    loadedPlugins.append(interface);
                }
                else {
                    QLOG_WARN() << "PluginManager :: " << "Skipping plugin " << fileName << ", because it has the wrong interface revision";
                }
            }
            else {
                 QLOG_ERROR() << "PluginManager :: " << "error loading lib" << loader.errorString();
            }
        }
        QLOG_TRACE() << "PluginManager :: " << "PluginList" << pluginList;
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

ParserPluginInterface* PluginManager::getPlugin(QString component) {
    ParserPluginInterface* ret;
    component_information c;

    ret = 0;

    c = components.value(component);

    for (int i=0; i<pluginList.count(); i++) {
        if (pluginList.at(i).endsWith(QString(";;;%1").arg(c.filename))) {
            ret = getPlugin(i);
            break;
        }
    }

    return ret;
}


QStringList PluginManager::getAvailablePlugins() {
    return components.keys();
}

component_information PluginManager::getInfo(QString name) {
    component_information ret;

    ret = components.value(name);

    return ret;
}

QStringList PluginManager::getSupportedDomains() {
    QStringList ret;

    for (int i=0; i<loadedPlugins.count(); i++) {
        ret << loadedPlugins.at(i)->getDomain();
    }

    return ret;
}

bool PluginManager::isSupported(QString domain) {
    bool ret;
    QStringList supported;

    ret = false;

    supported = getSupportedDomains();

    foreach (QString sd, supported) {
        if (domain.contains(sd)) {
            ret = true;

            break;
        }
    }

    return ret;
}
