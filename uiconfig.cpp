#include "uiconfig.h"
#include "ui_uiconfig.h"

UIConfig::UIConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIConfig)
{
    ui->setupUi(this);
}

void UIConfig::setSettings(QSettings* s) {
    bool b;

    settings = s;

    settings->beginGroup("options");
    ui->leDefaultSavepath->setText(settings->value("default_directory","").toString());
    ui->cmbTabPosition->setCurrentIndex(settings->value("tab_position",3).toInt());
    b = settings->value("automatic_close",false).toBool();
    if (b)
        ui->cbAutoClose->setChecked(true);
    else
        ui->cbAutoClose->setChecked(false);

    b = settings->value("resume_session",false).toBool();
    if (b)
        ui->cbReopenTabs->setChecked(true);
    else
        ui->cbReopenTabs->setChecked(false);


    b = settings->value("enlarge_thumbnails",false).toBool();
    if (b)
        ui->cbEnlargeThumbnails->setChecked(true);
    else
        ui->cbEnlargeThumbnails->setChecked(false);

    b = settings->value("hq_thumbnails",false).toBool();
    if (b)
        ui->cbHQThumbnail->setChecked(true);
    else
        ui->cbHQThumbnail->setChecked(false);

    ui->sbConcurrentDownloads->setValue(settings->value("concurrent_downloads",1).toInt());
    ui->sbThumbnailHeight->setValue(settings->value("thumbnail_height",200).toInt());
    ui->sbThumbnailWidth->setValue(settings->value("thumbnail_width",200).toInt());

    settings->endGroup();
}

void UIConfig::accept(void) {
    settings->beginGroup("options");
        settings->setValue("default_directory", ui->leDefaultSavepath->text());
        settings->setValue("tab_position", ui->cmbTabPosition->currentIndex());
        if (ui->cbAutoClose->isChecked())
            settings->setValue("automatic_close", true);
        else
            settings->setValue("automatic_close", false);

        if (ui->cbReopenTabs->isChecked())
            settings->setValue("resume_session", true);
        else
            settings->setValue("resume_session", false);

        if (ui->cbEnlargeThumbnails->isChecked())
            settings->setValue("enlarge_thumbnails", true);
        else
            settings->setValue("enlarge_thumbnails", false);

        if (ui->cbHQThumbnail->isChecked())
            settings->setValue("hq_thumbnails", true);
        else
            settings->setValue("hq_thumbnails", false);

        settings->setValue("concurrent_downloads", ui->sbConcurrentDownloads->value());
        settings->setValue("thumbnail_width", ui->sbThumbnailWidth->value());
        settings->setValue("thumbnail_height", ui->sbThumbnailHeight->value());
    settings->endGroup();
    settings->sync();

    emit configurationChanged();
    hide();
}

void UIConfig::reject(void) {
    hide();
}

void UIConfig::chooseLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose storage directory", ui->leDefaultSavepath->text());

    ui->leDefaultSavepath->setText(loc);
}

UIConfig::~UIConfig()
{
    delete ui;
}
