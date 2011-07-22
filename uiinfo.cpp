#include "uiinfo.h"
#include "ui_uiinfo.h"
UIInfo::UIInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIInfo)
{
    QString text;
    ui->setupUi(this);
    updateStatistics();

    timer = new QTimer(this);
    timer->setInterval(3000);
    timer->setSingleShot(false);
    timer->start();

    text = ui->label_2->text();
    text.replace("%%VERSION%%", PROGRAM_VERSION);
    text.replace("%%BUILDDATE%%", BUILDDATE);
    text.replace("%%QT_VERSION%%", QT_VERSION_STR);
    ui->label_2->setText(text);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatistics()));
}

void UIInfo::setCurrentVersion(QString s) {
    QString text;

    text = ui->label_2->text();
    text.replace("%%CURRENT_VERSION%%", s);
    ui->label_2->setText(text);
}

void UIInfo::updateStatistics() {
    QString s;
    int files;
    float kbyte;

    files = 0;
    kbyte = 0;

    downloadManager->getStatistics(&files, &kbyte);
    s = QString("You have downloaded\n%1 files\n%2 MB").arg(files).arg(kbyte/1024);

    ui->lStatistics->setText(s);
}

UIInfo::~UIInfo()
{
    delete ui;
}
