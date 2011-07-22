#ifndef UIINFO_H
#define UIINFO_H

#include <QDialog>
#include <QUrl>
#include <QtNetwork>
#include "downloadmanager.h"
//class DownloadManager;
extern DownloadManager* downloadManager;

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

private:
    Ui::UIInfo *ui;
    QTimer* timer;

private slots:
    void updateStatistics();

signals:
};

#endif // UIINFO_H
