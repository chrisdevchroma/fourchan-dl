#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QTreeWidget>
#include "ui4chan.h"
#include "uiinfo.h"
#include "uiconfig.h"
#include "applicationupdateinterface.h"
#include "defines.h"
#include "blacklist.h"
#include "thumbnailremoverthread.h"
#include "downloadmanager.h"
#include "uithreadadder.h"

class UI4chan;

extern DownloadManager* downloadManager;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void restoreTabs();
    bool threadExists(QString url);

private:
    Ui::MainWindow *ui;
    QList<UI4chan> widgetList;
    QMap<QString, QString> historyList;
    QString defaultDirectory;
    QSettings* settings;
    UIConfig* uiConfig;
    UIInfo* uiInfo;
    UIThreadAdder* threadAdder;
    ApplicationUpdateInterface* aui;
    BlackList* blackList;
    bool autoClose;
    QSize thumbnailSize;
    int maxDownloads;
    QNetworkAccessManager* manager;
    ThumbnailRemoverThread* thumbnailRemover;
    int oldActiveTabIndex;
    QTimer* overviewUpdateTimer;
    bool _updateOverview;

    void restoreWindowSettings(void);
    void saveSettings(void);
    void updateWidgetSettings(void);
    void checkVersion(QString ver);
    void addToHistory(QString s, QString title);
    void removeFromHistory(QString key);
protected:
    void keyPressEvent(QKeyEvent *event);
private slots:
    int addTab(void);
    void addMultipleTabs();
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
    void startAll(void);
    void stopAll(void);
    void pendingThumbnailsChanged(int);
    void showTab(QTreeWidgetItem*, int);
    void updateThreadOverview();
    void debugButton();
    void overviewTimerTimeout();
    void scheduleOverviewUpdate();
    void restoreFromHistory(QAction*);

signals:
    void removeFiles(QStringList);
};

#endif // MAINWINDOW_H
