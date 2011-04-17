#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include "ui4chan.h"
#include "uiinfo.h"
#include "uiconfig.h"
#include "applicationupdateinterface.h"
#include "defines.h"
#include "blacklist.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<UI4chan> widgetList;
    QString defaultDirectory;
    QSettings* settings;
    UIConfig* uiConfig;
    UIInfo* uiInfo;
    ApplicationUpdateInterface* aui;
    BlackList* blackList;
    bool autoClose;
    QSize thumbnailSize;
    int maxDownloads;
    QNetworkAccessManager* manager;

    void restoreWindowSettings(void);
    void saveSettings(void);
    void updateWidgetSettings(void);
    void checkVersion(QString ver);

private slots:
    int addTab(void);
    void closeTab(int);
    void displayError(QString);
    void changeTabTitle(UI4chan*, QString);
    void showInfo(void);
    void showConfiguration(void);
    void setDefaultDirectory(QString);
    void loadOptions(void);
    void processCloseRequest(UI4chan*, int);
    void newVersionAvailable(QString);
    void createTab(QString);
    void replyFinished(QNetworkReply*);
};

#endif // MAINWINDOW_H
