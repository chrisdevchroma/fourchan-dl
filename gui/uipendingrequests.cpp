#include "uipendingrequests.h"
#include "ui_uipendingrequests.h"

UIPendingRequests::UIPendingRequests(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIPendingRequests)
{
    ui->setupUi(this);
}

UIPendingRequests::~UIPendingRequests()
{
    delete ui;
}

void UIPendingRequests::showRequestList(QMap<qint64, QString> list) {
    QMapIterator<qint64, QString> i(list);
    QStringList data;
    ui->treeWidget->clear();

    while (i.hasNext()) {
        i.next();
        data.clear();
        data << QString("%1").arg(i.key()) << i.value();

        ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(ui->treeWidget, data));
    }
}

void UIPendingRequests::reload() {
    emit reloadRequested();
}
