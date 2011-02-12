#include "uiinfo.h"
#include "ui_uiinfo.h"
UIInfo::UIInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIInfo)
{
    QString text;

    ui->setupUi(this);

    text = ui->label_2->text();
    text.replace("%%VERSION%%", PROGRAM_VERSION);
    text.replace("%%BUILDDATE%%", BUILDDATE);
    text.replace("%%QT_VERSION%%", QT_VERSION_STR);
    ui->label_2->setText(text);
}

UIInfo::~UIInfo()
{
    delete ui;
}
