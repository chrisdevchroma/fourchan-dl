#ifndef UIINFO_H
#define UIINFO_H

#include <QDialog>
#include <QUrl>
#include <QtNetwork>
#include <QFile>
#include "downloadmanager.h"
#include "pluginmanager.h"

extern DownloadManager* downloadManager;
extern PluginManager* pluginManager;

namespace Ui {
    class UIInfo;
}

class UIInfo : public QDialog
{
    Q_OBJECT

public:
    explicit UIInfo(QWidget *parent = 0);
    ~UIInfo();
    void setCurrentVersion(QString);
    void loadComponentInfo(QMap<QString, component_information> components);

private:
    Ui::UIInfo *ui;
    QTimer* timer;
    QFile* logFile;


private slots:
    void updateStatistics();
    void updateDebugInformation();
    void updateLogFile();

signals:
};

#endif // UIINFO_H
