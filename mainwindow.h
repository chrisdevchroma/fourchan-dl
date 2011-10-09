#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QTreeWidget>
#include "uiimageoverview.h"
#include "uiinfo.h"
#include "uiconfig.h"
#include "applicationupdateinterface.h"
#include "defines.h"
#include "blacklist.h"
#include "thumbnailremoverthread.h"
#include "downloadmanager.h"
#include "uithreadadder.h"

class UIImageOverview;

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
    ThumbnailRemoverThread* thumbnailRemover;
    int oldActiveTabIndex;
    QTimer* overviewUpdateTimer;
    QMenu* historyMenu;
    bool _updateOverview;
    QMap<QString, component_information> components;
    QList<QString> updateableComponents;
    bool runUpdate;

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

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void saveSettings(void);
    int addTab(void);
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
    void stopAll(void);
    void pendingThumbnailsChanged(int);
    void showTab(QTreeWidgetItem*, int);
    void updateThreadOverview();
    void debugButton();
    void overviewTimerTimeout();
    void scheduleOverviewUpdate();
    void restoreFromHistory(QAction*);
    void processRequestResponse(QUrl url, QByteArray ba);
    void handleRequestError(QUrl url, int error);
    void updaterConnected();
    void updateFinished();
    void setUpdaterVersion(QString);

signals:
    void removeFiles(QStringList);
};

#endif // MAINWINDOW_H
