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

    dialogFolderShortcut = new DialogFolderShortcut(this);
    dialogFolderShortcut->setModal(true);

    loadSettings();
    loadShortcuts();

    connect(timeoutValueEditor, SIGNAL(valuesChanged()), this, SLOT(loadSettings()));
    connect(ui->cbUseProxy, SIGNAL(toggled(bool)), this, SLOT(toggleProxy(bool)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editShortcutItem(QListWidgetItem*)));
    connect(folderShortcuts, SIGNAL(shortcutsChanged()), this, SLOT(loadShortcuts()));
    connect(dialogFolderShortcut, SIGNAL(shortcutChanged(QString,QString,QString)), folderShortcuts, SLOT(updateShortcut(QString,QString,QString)));
    connect(dialogFolderShortcut, SIGNAL(editCanceled()), this, SLOT(loadShortcuts()));
    connect(ui->btnDeleteAllThumbnails, SIGNAL(clicked()), this, SIGNAL(deleteAllThumbnails()));
}

UIConfig::~UIConfig()
{
    delete ui;
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

    ui->cbCloseOverviewThreads->setChecked(settings->value("close_overview_threads", true).toBool());
    ui->cbUseInternalViewer->setChecked(settings->value("use_internal_viewer", false).toBool());

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

    ui->cbCloseToTray->setChecked(settings->value("close_to_tray", false).toBool());
    ui->cbLoggingLevel->setCurrentIndex(settings->value("log_level",3).toInt());
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

    settings->beginGroup("download_manager");
        ui->sbConcurrentDownloads->setValue(settings->value("concurrent_downloads",20).toInt());
        ui->sbDownloadTimeoutInitial->setValue(settings->value("initial_timeout",30).toInt());
        ui->sbDownloadTimeoutInbetween->setValue(settings->value("running_timeout",2).toInt());
        ui->cbKeepLocalHTMLCopy->setChecked(settings->value("use_thread_cache", false).toBool());
        ui->leThreadCachePath->setText(settings->value("thread_cache_path", "").toString());
    settings->endGroup();

    ui->sbUpdaterPort->setValue(settings->value("updater/updater_port", 60000).toInt());
    ui->sbApplicationPort->setValue(settings->value("updater/application_port", 60001).toInt());

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

        settings->setValue("concurrent_downloads", ui->sbConcurrentDownloads->value());
        settings->setValue("reschedule_interval", ui->sbRescheduleInterval->value());
        settings->setValue("thumbnail_width", ui->sbThumbnailWidth->value());
        settings->setValue("thumbnail_height", ui->sbThumbnailHeight->value());

        settings->setValue("default_timeout", ui->cbRescanInterval->itemData(ui->cbRescanInterval->currentIndex()));

        settings->setValue("thumbnail_cache_folder", ui->leThumbnailCacheFolder->text());
        settings->setValue("thumbnail_TTL", ui->sbThumbnailTTL->value());

        settings->setValue("close_overview_threads", ui->cbCloseOverviewThreads->isChecked());
        settings->setValue("use_internal_viewer", ui->cbUseInternalViewer->isChecked());
        settings->setValue("close_to_tray", ui->cbCloseToTray->isChecked());

        settings->setValue("log_level", ui->cbLoggingLevel->currentIndex());
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

    settings->beginGroup("download_manager");
        settings->setValue("concurrent_downloads", ui->sbConcurrentDownloads->value());
        settings->setValue("initial_timeout", ui->sbDownloadTimeoutInitial->value());
        settings->setValue("running_timeout", ui->sbDownloadTimeoutInbetween->value());
        settings->setValue("use_thread_cache", ui->cbKeepLocalHTMLCopy->isChecked());
        settings->setValue("thread_cache_path", ui->leThreadCachePath->text());
    settings->endGroup();

    settings->setValue("updater/updater_port", ui->sbUpdaterPort->value());
    settings->setValue("updater/application_port", ui->sbApplicationPort->value());

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

    loc = QFileDialog::getExistingDirectory(this, "Choose thumbnail cache directory", ui->leThumbnailCacheFolder->text());
    if (!loc.isEmpty())
        ui->leThumbnailCacheFolder->setText(loc);
}

void UIConfig::chooseThreadCacheLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose thread cache directory", ui->leThreadCachePath->text());
    if (!loc.isEmpty())
        ui->leThreadCachePath->setText(loc);
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

void UIConfig::addShortcut() {
//    ui->listWidget->addItem("???");
//    editShortcutItem(ui->listWidget->item(ui->listWidget->count()-1));
    editShortcut("");
}

void UIConfig::editShortcut(QString name) {
    dialogFolderShortcut->clear();
    dialogFolderShortcut->edit(name);

    dialogFolderShortcut->show();
}

void UIConfig::editShortcutItem(QListWidgetItem* item) {
    editShortcut(item->text());
}

void UIConfig::loadShortcuts() {
    ui->listWidget->clear();

    ui->listWidget->addItems(folderShortcuts->shortcuts());
}

void UIConfig::deleteShortcut() {
    QString name;

    name = ui->listWidget->currentItem()->text();

    if (name != "???")
        folderShortcuts->deleteShortcut(name);
    else
        ui->listWidget->removeItemWidget(ui->listWidget->currentItem());
}

void UIConfig::toggleLogLevelWarning(QString s) {
    if (s == "Trace") {
        ui->lLogLevelWarning->show();
    }
    else {
        ui->lLogLevelWarning->hide();
    }
}
