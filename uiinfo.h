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
    void setCurrentVersion(QString);

private:
    Ui::UIInfo *ui;

signals:
};

#endif // UIINFO_H
