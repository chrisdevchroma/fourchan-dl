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
    ui->label_2->setText(text);
}

UIInfo::~UIInfo()
{
    delete ui;
}
