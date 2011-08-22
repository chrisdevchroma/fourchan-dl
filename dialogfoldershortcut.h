#ifndef DIALOGFOLDERSHORTCUT_H
#define DIALOGFOLDERSHORTCUT_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include "foldershortcuts.h"

extern FolderShortcuts* folderShortcuts;

namespace Ui {
    class DialogFolderShortcut;
}

class DialogFolderShortcut : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFolderShortcut(QWidget *parent = 0);
    ~DialogFolderShortcut();
    void edit(QString);

private:
    Ui::DialogFolderShortcut *ui;

public slots:
    void clear();

private slots:
    void selectShortcutIndex(int);
    void selectShortcut(QString);
    void choosePath();
    void checkValues();
    void accept(void);
    void reject(void);
    void fillShortcutComboBox();

signals:
    void shortcutChanged(QString, QString);
    void editCanceled();
};

#endif // DIALOGFOLDERSHORTCUT_H
