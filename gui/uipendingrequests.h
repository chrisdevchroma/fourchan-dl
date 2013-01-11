#ifndef UIPENDINGREQUESTS_H
#define UIPENDINGREQUESTS_H

#include <QDialog>
#include <QMap>
#include <QString>

namespace Ui {
class UIPendingRequests;
}

class UIPendingRequests : public QDialog
{
    Q_OBJECT
    
public:
    explicit UIPendingRequests(QWidget *parent = 0);
    ~UIPendingRequests();
    
private:
    Ui::UIPendingRequests *ui;

private slots:
    void reload();

public slots:
    void showRequestList(QMap<qint64, QString>);

signals:
    void reloadRequested();
};

#endif // UIPENDINGREQUESTS_H
