#ifndef UITHREADADDER_H
#define UITHREADADDER_H

#include <QDialog>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QtDebug>
#include <QFileDialog>
#include <QSettings>
#include "foldershortcuts.h"

extern FolderShortcuts* folderShortcuts;

namespace Ui {
    class UIThreadAdder;
}

class UIThreadAdder : public QDialog
{
    Q_OBJECT

public:
    explicit UIThreadAdder(QWidget *parent = 0);
    ~UIThreadAdder();

private:
    Ui::UIThreadAdder *ui;
    QClipboard* clipboard;
    QSettings* settings;
    QList<int> timeoutValues;

    void addLinks(QStringList links);
    QStringList parseText(QString text);
    QStringList parseHTML(QString html);
    QStringList parseURLList(QList<QUrl> urls);
    void loadSettings();

private slots:
    void checkClipboard();
    void selectAll();
    void selectNone();
    void startSelected();
    void startAll();
    bool readyToStart();
    void chooseLocation();
    void clearSelected();
    void selectShortcutIndex(int idx);
    void selectShortcut(QString name);
    void fillShortcutComboBox();

signals:
    void directoryChanged(QString);
    void addTab(QString);
};

#endif // UITHREADADDER_H
