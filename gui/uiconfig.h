#ifndef UICONFIG_H
#define UICONFIG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include "uilisteditor.h"
#include "dialogfoldershortcut.h"
#include "foldershortcuts.h"
#include "QsLog.h"

extern FolderShortcuts* folderShortcuts;

namespace Ui {
    class UIConfig;
}

class UIConfig : public QDialog
{
    Q_OBJECT

public:
    explicit UIConfig(QWidget *parent = 0);
    ~UIConfig();

private:
    Ui::UIConfig *ui;
    QSettings* settings;
    UIListEditor* timeoutValueEditor;
    DialogFolderShortcut* dialogFolderShortcut;

private slots:
    void accept(void);
    void reject(void);
    void chooseLocation(void);
    void chooseThumbnailCacheLocation(void);
    void chooseThreadCacheLocation(void);
    void editTimeoutValues(void);
    void loadSettings(void);
    void toggleProxy(bool);
    void addShortcut(void);
    void editShortcut(QString);
    void deleteShortcut();
    void editShortcutItem(QListWidgetItem*);
    void loadShortcuts();
    void toggleLogLevelWarning(QString);
signals:
    void configurationChanged(void);
    void deleteAllThumbnails();
};

#endif // UICONFIG_H
