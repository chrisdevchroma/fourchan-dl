#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSettings>
#include "ui4chan.h"
#include "uiinfo.h"
#include "uiconfig.h"

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
    bool autoClose;
    QSize thumbnailSize;
    int maxDownloads;

    void restoreSettings(void);
    void saveSettings(void);
    void updateThumbnailSize(void);
    void updateMaxDownloads(void);

private slots:
    int addTab(void);
    void closeTab(int);
    void displayError(QString);
    void changeTabTitle(UI4chan*, QString);
    void showInfo(void);
    void showConfiguration(void);
    void setDefaultDirectory(QString);
    void loadOptions(void);
    void processCloseRequest(UI4chan*);
};

#endif // MAINWINDOW_H
