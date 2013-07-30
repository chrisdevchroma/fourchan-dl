#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QTreeWidget>
#include <QSystemTrayIcon>
#include <QThread>
#include "uiimageoverview.h"
#include "uiinfo.h"
#include "uiconfig.h"
#include "applicationupdateinterface.h"
#include "ecwin7.h"
#include "defines.h"
#include "blacklist.h"
#include "downloadmanager.h"
#include "uithreadadder.h"
#include "thumbnailremover.h"
#include "QsLog.h"

class UIImageOverview;

extern DownloadManager* downloadManager;
extern QString updaterFileName;

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
    QList<component_information> getComponents();

private:
    Ui::MainWindow *ui;
    QList<UIImageOverview> widgetList;
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
    RequestHandler* requestHandler;
    QThread* thumbnailRemoverThread;
    ThumbnailRemover* thumbnailRemover;
    int oldActiveTabIndex;
    QTimer* overviewUpdateTimer;
    QMenu* historyMenu;
    bool _updateOverview;
    QMap<QString, component_information> components;
    QList<QString> updateableComponents;
    bool runUpdate;
    bool checkUpdaterVersion;
    EcWin7 win7;
    bool _paused;

    void restoreWindowSettings(void);
    void updateWidgetSettings(void);
    void checkVersion(QString ver);
    void checkForUpdates(QString xml);
    bool checkIfNewerVersion(QString _new, QString _old);
    void addToHistory(QString s, QString title);
    void removeFromHistory(QString key);

    void createSupervisedDownload(QUrl);
    void removeSupervisedDownload(QUrl);

    void createComponentList();
    void newComponentsAvailable();

    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void createTrayActions();
    void createTrayIcon();

    QTimer* autosaveTimer;

    void cleanThreadCache();

protected:
    void keyPressEvent(QKeyEvent *event);
#ifdef Q_OS_WIN
    virtual bool winEvent(MSG *message, long *result);
#endif

private slots:
    void saveSettings(void);
    int addTab();
    int addForegroundTab();
    void addMultipleTabs();
    void closeTab(int);
    void displayError(QString);
    void changeTabTitle(UIImageOverview*, QString);
    void showInfo(void);
    void showConfiguration(void);
    void setDefaultDirectory(QString);
    void loadOptions(void);
    void processCloseRequest(UIImageOverview*, int);
    void createTab(QString);
    void startAll(void);
    void pauseAll(void);
    void stopAll(void);
    void pendingThumbnailsChanged(int);
    void showTab(QTreeWidgetItem*, int);
    void updateThreadOverview();
    void debugButton();
    void overviewTimerTimeout();
    void scheduleOverviewUpdate();
    void restoreFromHistory(QAction*);
    void processRequestResponse(QUrl url, QByteArray ba, bool cached);
    void updaterConnected();
    void updateFinished();
    void setUpdaterVersion(QString);
    void updateDownloadProgress();
    void removeTrayIcon();
    void trayIconActivated(QSystemTrayIcon::ActivationReason ar);
    void toggleThreadOverview();
    void aboutToQuit();
    void removeThreadOverviewMark();
    void addThreadOverviewMark(QTreeWidgetItem *item);
    void addThreadOverviewMark(int index);
    void getUpdaterVersion();

signals:
    void removeFiles(QStringList);
    void quitAll();
};

#endif // MAINWINDOW_H
