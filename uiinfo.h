#ifndef UIINFO_H
#define UIINFO_H

#include <QDialog>
#include <QUrl>
#include <QtNetwork>

namespace Ui {
    class UIInfo;
}

class UIInfo : public QDialog
{
    Q_OBJECT

public:
    explicit UIInfo(QWidget *parent = 0);
    ~UIInfo();

private:
    void checkVersion(QString ver);

    Ui::UIInfo *ui;
    QNetworkAccessManager* manager;

private slots:
    void replyFinished(QNetworkReply*);

signals:
    void newerVersionAvailable(QString);
};

#endif // UIINFO_H
