#include "uilisteditor.h"
#include "ui_uilisteditor.h"

UIListEditor::UIListEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIListEditor)
{
    ui->setupUi(this);

    settings = new QSettings("settings.ini", QSettings::IniFormat);
    loadSettings();

    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editItem(QListWidgetItem*)));
//    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(sortList()));
}

void UIListEditor::accept() {
    saveSettings();
    hide();
    emit valuesChanged();
}

void UIListEditor::reject() {
    hide();
    loadSettings();
}

void UIListEditor::addItem() {
    ui->listWidget->addItem("???");

    editItem(ui->listWidget->item(ui->listWidget->count()-1));
}

void UIListEditor::removeItem() {
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void UIListEditor::editItem(QListWidgetItem *item) {
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->listWidget->editItem(item);
}

void UIListEditor::sortList() {
    QList<int> intList;

    intList.clear();

    for (int i=0; i < ui->listWidget->count(); i++) {
        int tmp;
        bool ok;

        tmp = ui->listWidget->item(i)->text().toInt(&ok);
        if (ok)
            intList.append(tmp);
    }
    qSort(intList.begin(), intList.end());

    ui->listWidget->clear();

    foreach (int i, intList) {
        ui->listWidget->addItem(QString("%1").arg(i));
    }
}

void UIListEditor::loadSettings() {
    QStringList sl;

    sl = settings->value("options/timeout_values", (QStringList()<<"30"<<"60"<<"120"<<"300"<<"600")).toStringList();
    ui->listWidget->clear();
    ui->listWidget->addItems(sl);

    sortList();
}

void UIListEditor::saveSettings() {
    QStringList sl;

    sortList();

    for (int i=0; i < ui->listWidget->count(); i++) {
        sl << ui->listWidget->item(i)->text();
    }

    settings->setValue("options/timeout_values", sl);
}

UIListEditor::~UIListEditor()
{
    delete ui;
}
