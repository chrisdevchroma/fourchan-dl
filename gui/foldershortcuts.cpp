#include "foldershortcuts.h"

FolderShortcuts::FolderShortcuts(QObject *parent) :
    QObject(parent)
{
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    loadSettings();
}

QStringList FolderShortcuts::shortcuts() {
    return _shortcuts.keys();
}

bool FolderShortcuts::shortcutExists(QString name) {
    bool ret;


    ret = false;

    if (_shortcuts.value(name, "-1") == "-1") {
        ret = false;
    }
    else {
        ret = true;
    }

    return ret;
}

QString FolderShortcuts::getPath(QString name) {
    return _shortcuts.value(name, "");
}

bool FolderShortcuts::addShortcut(QString name, QString path) {
    bool ret;

    if (shortcutExists(name)) {
        ret = false;
    }
    else {
        ret = true;
        _shortcuts.insert(name, path);
        emit shortcutsChanged();
        saveSettings();
    }

    return ret;
}

void FolderShortcuts::updateShortcut(QString originalName, QString newName, QString path) {
    if (shortcutExists(originalName)) {
        deleteShortcut(originalName);
    }

    addShortcut(newName, path);
}

void FolderShortcuts::deleteShortcut(QString name) {
    _shortcuts.remove(name);
    emit shortcutsChanged();
    saveSettings();
}

void FolderShortcuts::loadSettings() {
    QStringList sl;
    QStringList temp;

    sl = settings->value("shortcuts", "").toStringList();

    foreach (QString s, sl) {
        temp = s.split("=>");
        if (temp.count() == 2) {
            _shortcuts.insert(temp.at(0), temp.at(1));
        }
    }

    emit shortcutsChanged();
}

void FolderShortcuts::saveSettings() {
    QStringList s;

    QMap<QString, QString>::iterator i;

    for(i=_shortcuts.begin(); i!= _shortcuts.end(); ++i)
        s << QString("%1=>%2").arg(i.key()).arg(i.value());

    settings->setValue("shortcuts", s);
}
