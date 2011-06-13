#include "uiconfig.h"
#include "ui_uiconfig.h"

UIConfig::UIConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIConfig)
{
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    timeoutValueEditor = new UIListEditor(this);
    timeoutValueEditor->setModal(true);

    loadSettings();

    connect(timeoutValueEditor, SIGNAL(valuesChanged()), this, SLOT(loadSettings()));
    connect(ui->cbUseProxy, SIGNAL(toggled(bool)), this, SLOT(toggleProxy(bool)));
}

void UIConfig::loadSettings(void) {
    QStringList sl;
    int index;
    bool b;

    settings->beginGroup("options");
    ui->leDefaultSavepath->setText(settings->value("default_directory","").toString());
    ui->cmbTabPosition->setCurrentIndex(settings->value("tab_position",3).toInt());
    b = settings->value("automatic_close",false).toBool();
        ui->cbAutoClose->setChecked(b);

    b = settings->value("resume_session",false).toBool();
        ui->cbReopenTabs->setChecked(b);

    b = settings->value("enlarge_thumbnails",false).toBool();
        ui->cbEnlargeThumbnails->setChecked(b);

    b = settings->value("hq_thumbnails",false).toBool();
        ui->cbHQThumbnail->setChecked(b);

    b = settings->value("default_original_filename",false).toBool();
        ui->cbDefaultOriginalFilename->setChecked(b);

    b = settings->value("remember_directory",false).toBool();
        ui->cbRememberDirectory->setChecked(b);

    b = settings->value("use_thumbnail_cache",true).toBool();
        ui->cbUseThumbnailCache->setChecked(b);

    ui->sbConcurrentDownloads->setValue(settings->value("concurrent_downloads",1).toInt());
    ui->sbRescheduleInterval->setValue(settings->value("reschedule_interval", 60).toInt());
    ui->sbThumbnailHeight->setValue(settings->value("thumbnail_height",200).toInt());
    ui->sbThumbnailWidth->setValue(settings->value("thumbnail_width",200).toInt());
    ui->leThumbnailCacheFolder->setText(settings->value("thumbnail_cache_folder", QString("%1/%2").arg(QCoreApplication::applicationDirPath())
                                                        .arg("tncache")).toString());
    ui->sbThumbnailTTL->setValue(settings->value("thumbnail_TTL", 60).toInt());

    sl = settings->value("timeout_values", (QStringList()<<"30"<<"60"<<"120"<<"300"<<"600")).toStringList();
    ui->cbRescanInterval->clear();
    ui->cbRescanInterval->addItem("Never", 0);
    foreach (QString s, sl) {
        int value;
        int i;
        bool ok;
        QString text;

        i = s.toInt(&ok);
        if (ok) {
            if (i > 60 && ((i%60) == 0)) {              // Display as minutes
                if (i > 3600 && ((i%3600) == 0)) {      // Display as hours
                    if (i > 86400 && ((i%86400)==0)) {  // Display as days
                        value = i/86400;
                        text = "days";
                    }
                    else {
                        value = i/3600;
                        text = "hours";
                    }
                }
                else {
                    value = i/60;
                    text = "minutes";
                }
            }
            else {
                value = i;
                text = "seconds";
            }

            ui->cbRescanInterval->addItem(QString("every %1 %2").arg(value).arg(text), i);
        }
    }

    index = ui->cbRescanInterval->findData(settings->value("default_timeout", 0).toInt());
    if (index != -1) ui->cbRescanInterval->setCurrentIndex(index);
    else ui->cbRescanInterval->setCurrentIndex(0);
    settings->endGroup();

    settings->beginGroup("blacklist");
    b = settings->value("use_blacklist",true).toBool();
    ui->cbUseBlackList->setChecked(b);
    ui->sbBlackListCheckInterval->setValue(settings->value("blacklist_check_interval", 600).toInt());
    settings->endGroup();

    settings->beginGroup("network");
        b = settings->value("use_proxy", false).toBool();
        ui->cbUseProxy->setChecked(b);
        ui->cbProxyAuth->setChecked(settings->value("proxy_auth", false).toBool());
        toggleProxy(b);

        ui->leProxyAddress->setText(settings->value("proxy_hostname", "").toString());
        ui->sbProxyPort->setValue(settings->value("proxy_port", 0).toInt());
        ui->leProxyUser->setText(settings->value("proxy_user", "").toString());
        ui->leProxyPassword->setText(settings->value("proxy_pass", "").toString());

        int proxyType;
        proxyType = settings->value("proxy_type", 0).toInt();
        if (proxyType>=2) proxyType--;
        ui->cbProxyType->setCurrentIndex(proxyType);
    settings->endGroup();

    timeoutValueEditor->loadSettings();
}

void UIConfig::accept(void) {
    settings->beginGroup("options");
        settings->setValue("default_directory", ui->leDefaultSavepath->text());
        settings->setValue("tab_position", ui->cmbTabPosition->currentIndex());

        settings->setValue("automatic_close", ui->cbAutoClose->isChecked());
        settings->setValue("resume_session", ui->cbReopenTabs->isChecked());
        settings->setValue("enlarge_thumbnails", ui->cbEnlargeThumbnails->isChecked());
        settings->setValue("hq_thumbnails", ui->cbHQThumbnail->isChecked());
        settings->setValue("default_original_filename", ui->cbDefaultOriginalFilename->isChecked());
        settings->setValue("remember_directory", ui->cbRememberDirectory->isChecked());
        settings->setValue("use_thumbnail_cache", ui->cbUseThumbnailCache->isChecked());

        settings->setValue("concurrent_downloads", ui->sbConcurrentDownloads->value());
        settings->setValue("reschedule_interval", ui->sbRescheduleInterval->value());
        settings->setValue("thumbnail_width", ui->sbThumbnailWidth->value());
        settings->setValue("thumbnail_height", ui->sbThumbnailHeight->value());

        settings->setValue("default_timeout", ui->cbRescanInterval->itemData(ui->cbRescanInterval->currentIndex()));

        settings->setValue("thumbnail_cache_folder", ui->leThumbnailCacheFolder->text());
        settings->setValue("thumbnail_TTL", ui->sbThumbnailTTL->value());
    settings->endGroup();
    settings->beginGroup("blacklist");
        if (ui->cbUseBlackList->isChecked())
            settings->setValue("use_blacklist", true);
        else
            settings->setValue("use_blacklist", false);

        settings->setValue("blacklist_check_interval", ui->sbBlackListCheckInterval->value());
    settings->endGroup();

    settings->beginGroup("network");
        settings->setValue("use_proxy", ui->cbUseProxy->isChecked());
        settings->setValue("proxy_auth", ui->cbProxyAuth->isChecked());
        settings->setValue("proxy_hostname",  ui->leProxyAddress->text());
        settings->setValue("proxy_port",  ui->sbProxyPort->value());
        settings->setValue("proxy_user",  ui->leProxyUser->text());
        settings->setValue("proxy_pass",  ui->leProxyPassword->text());

        int proxyType;

        proxyType = ui->cbProxyType->currentIndex();
        if (proxyType >= 2) proxyType++;

        settings->setValue("proxy_type", proxyType);
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
    if (!loc.isEmpty())
        ui->leDefaultSavepath->setText(loc);
}

void UIConfig::chooseThumbnailCacheLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose storage directory", ui->leThumbnailCacheFolder->text());
    if (!loc.isEmpty())
        ui->leThumbnailCacheFolder->setText(loc);
}

void UIConfig::editTimeoutValues(void) {
    timeoutValueEditor->show();
}

void UIConfig::toggleProxy(bool b) {
    ui->leProxyAddress->setEnabled(b);
    ui->sbProxyPort->setEnabled(b);
    ui->cbProxyType->setEnabled(b);
    ui->cbProxyAuth->setEnabled(b);

    if (b) {
        if (ui->cbProxyAuth->isChecked()) {
            ui->leProxyUser->setEnabled(b);
            ui->leProxyPassword->setEnabled(b);
        }
    }
    else {
        ui->leProxyUser->setEnabled(b);
        ui->leProxyPassword->setEnabled(b);
    }
}

UIConfig::~UIConfig()
{
    delete ui;
}
