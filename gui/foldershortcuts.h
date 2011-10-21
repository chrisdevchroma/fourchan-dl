#ifndef FOLDERSHORTCUTS_H
#define FOLDERSHORTCUTS_H

#include <QObject>
#include <QSettings>
#include <QStringList>

class FolderShortcuts : public QObject
{
    Q_OBJECT
public:
    explicit FolderShortcuts(QObject *parent = 0);
    QStringList shortcuts();
    QString getPath(QString);
    bool shortcutExists(QString);
private:
    QSettings* settings;
    QMap<QString, QString> _shortcuts;

    void loadSettings();
    void saveSettings();
signals:

public slots:
    bool addShortcut(QString name, QString path);
    void updateShortcut(QString originalName, QString newName, QString path);
    void deleteShortcut(QString name);

signals:
    void shortcutsChanged();
};

#endif // FOLDERSHORTCUTS_H
