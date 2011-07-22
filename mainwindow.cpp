#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    uiConfig = new UIConfig(this);
    uiInfo = new UIInfo(this);
    threadAdder = new UIThreadAdder(this);
    aui = new ApplicationUpdateInterface(this);
    manager = new QNetworkAccessManager();
    blackList = new BlackList(this);
    thumbnailRemover = new ThumbnailRemoverThread(this);

    thumbnailRemover->start(QThread::LowPriority);

    connect(this, SIGNAL(removeFiles(QStringList)), thumbnailRemover, SLOT(removeFiles(QStringList)));

//    downloadManager = new DownloadManager(this);
//    downloadManager->start(QThread::NormalPriority);

    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    ui->tabWidget->removeTab(0);
    oldActiveTabIndex = 0;
    pendingThumbnailsChanged(0);

    loadOptions();
    restoreWindowSettings();
    updateWidgetSettings();

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(uiConfig, SIGNAL(configurationChanged()), this, SLOT(loadOptions()));
    connect(uiConfig, SIGNAL(configurationChanged()), blackList, SLOT(loadSettings()));
    connect(uiConfig, SIGNAL(configurationChanged()), downloadManager, SLOT(loadSettings()));
    connect(ui->actionStart_all, SIGNAL(triggered()), this, SLOT(startAll()));
    connect(ui->actionStop_all, SIGNAL(triggered()), this, SLOT(stopAll()));

    connect(downloadManager, SIGNAL(error(QString)), ui->statusBar, SLOT(showMessage(QString)));

//    if (tnt->isRunning()) {
        connect(tnt, SIGNAL(pendingThumbnails(int)), ui->pbPendingThumbnails, SLOT(setValue(int)));
        connect(tnt, SIGNAL(pendingThumbnails(int)), this, SLOT(pendingThumbnailsChanged(int)));
//    }
//    connect(downloadManager, SIGNAL(totalRequestsChanged(int)), ui->pbOpenRequests, SLOT(setMaximum(int)));
//    connect(downloadManager, SIGNAL(finishedRequestsChanged(int)), ui->pbOpenRequests, SLOT(setValue(int)));

    manager->get(QNetworkRequest(QUrl("http://sourceforge.net/projects/fourchan-dl/files/")));
}

int MainWindow::addTab() {
    int ci;
    UI4chan* w;

    w = new UI4chan(this);
    w->setBlackList(blackList);

//    w->setDownloadManager(downloadManager);

    ci = ui->tabWidget->addTab(w, "no name");
    if (settings->value("options/remember_directory", false).toBool())
        w->setDirectory(defaultDirectory);

    connect(w, SIGNAL(errorMessage(QString)), this, SLOT(displayError(QString)));
    connect(w, SIGNAL(tabTitleChanged(UI4chan*, QString)), this, SLOT(changeTabTitle(UI4chan*, QString)));
    connect(w, SIGNAL(closeRequest(UI4chan*, int)), this, SLOT(processCloseRequest(UI4chan*, int)));
    connect(w, SIGNAL(directoryChanged(QString)), this, SLOT(setDefaultDirectory(QString)));
    connect(w, SIGNAL(createTabRequest(QString)), this, SLOT(createTab(QString)));
    connect(w, SIGNAL(removeFiles(QStringList)), this, SIGNAL(removeFiles(QStringList)));

    ui->tabWidget->setCurrentIndex(ci);

    changeTabTitle(w, "idle");

    return ci;
}

void MainWindow::createTab(QString s) {
    int index;
    UI4chan* w;

    index = addTab();
    w = ((UI4chan*)ui->tabWidget->widget(index));
    w->setValues(s);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
}

void MainWindow::closeTab(int i) {
    UI4chan* w;

    ui->tabWidget->setCurrentIndex(i);
    w = (UI4chan*)ui->tabWidget->widget(i);
    if (w->close()) {
        ui->tabWidget->removeTab(i);
        w->deleteLater();
    }
    else
        qDebug() << "Close widget event not accepted";

    if (ui->tabWidget->count() == 0) {
        addTab();
    }
}

void MainWindow::displayError(QString s) {
    ui->statusBar->showMessage(s, 10000);
}

void MainWindow::showInfo(void) {
    uiInfo->show();
}

void MainWindow::showConfiguration(void) {
    uiConfig->show();
}

void MainWindow::setDefaultDirectory(QString d) {
    if (settings->value("options/remember_directory", false).toBool())
        defaultDirectory = d;
}

void MainWindow::changeTabTitle(UI4chan* w, QString s) {
    int i;

    i = ui->tabWidget->indexOf((QWidget*)w);
    ui->tabWidget->setTabText(i, s);
}

void MainWindow::restoreWindowSettings(void) {
    // Restore window position
    QPoint p;
    QSize s;
    int state;

    settings->beginGroup("window");
        p = settings->value("position",QPoint(0,0)).toPoint();
        state = settings->value("state",0).toInt();
        s = settings->value("size",QSize(0,0)).toSize();
    settings->endGroup();

    if (p != QPoint(0,0))
        this->move(p);

    if (s != QSize(0,0))
        this->resize(s);

    if (state != Qt::WindowNoState)
        this->setWindowState((Qt::WindowState) state);
}

void MainWindow::restoreTabs() {
    int tabCount;

    tabCount = settings->value("tabs/count",0).toInt();

    ui->pbOpenRequests->setMaximum(tabCount);
    ui->pbOpenRequests->setValue(0);
    ui->pbOpenRequests->setFormat("Opening tab %v/%m");
    ui->tabWidget->setVisible(false);
    if (settings->value("options/resume_session", false).toBool() && tabCount > 0) {
        int ci;

        for (int i=0; i<tabCount; i++) {
            ci = addTab();

            ((UI4chan*)ui->tabWidget->widget(ci))->setValues(
                    settings->value(QString("tabs/tab%1").arg(i), ";;;;0;;every 30 seconds;;0").toString()
                    );
            ui->pbOpenRequests->setValue((i+1));
        }
    } else {
        addTab();
    }

    ui->tabWidget->setVisible(true);
    ui->pbOpenRequests->setVisible(false);
//    ui->pbOpenRequests->setFormat("%v/%m (%p%) requests finished");
//    ui->pbOpenRequests->setValue(0);
//    ui->pbOpenRequests->setMaximum(0);
}

void MainWindow::saveSettings(void) {
    int downloadedFiles;
    float downloadedKB;
    // Window related stuff
    settings->beginGroup("window");
        settings->setValue("position", this->pos());
        if (this->windowState() == Qt::WindowNoState)
            settings->setValue("size", this->size());
        settings->setValue("state", QString("%1").arg(this->windowState()));
    settings->endGroup();

    // Options
    settings->beginGroup("options");
    settings->endGroup();

    // Active tabs
    settings->beginGroup("tabs");
        settings->setValue("count", ui->tabWidget->count());

        for (int i=0; i<ui->tabWidget->count(); i++) {
            settings->setValue(QString("tab%1").arg(i), ((UI4chan*)ui->tabWidget->widget(i))->getValues());
        }
    settings->endGroup();

    downloadManager->getStatistics(&downloadedFiles, &downloadedKB);
    settings->beginGroup("statistics");
        settings->setValue("downloaded_files", downloadedFiles);
        settings->setValue("downloaded_kbytes", downloadedKB);
    settings->endGroup();

    settings->sync();
}

void MainWindow::loadOptions(void) {
    settings->beginGroup("options");
        defaultDirectory = settings->value("default_directory", "").toString();
        ui->tabWidget->setTabPosition((QTabWidget::TabPosition)settings->value("tab_position", 3).toInt());
        autoClose = settings->value("automatic_close", false).toBool();
        thumbnailSize.setWidth(settings->value("thumbnail_width", 200).toInt());
        thumbnailSize.setHeight(settings->value("thumbnail_height", 200).toInt());
        maxDownloads = settings->value("concurrent_downloads", 1).toInt();

        updateWidgetSettings();
        tnt->setIconSize(QSize(settings->value("thumbnail_width",150).toInt(),settings->value("thumbnail_height",150).toInt()));
    settings->endGroup();

    settings->beginGroup("network");
    QNetworkProxy proxy;

    if (settings->value("use_proxy", false).toBool()) {
        proxy.setType((QNetworkProxy::ProxyType)(settings->value("proxy_type", QNetworkProxy::HttpProxy).toInt()));
        proxy.setHostName(settings->value("proxy_hostname", "").toString());
        proxy.setPort(settings->value("proxy_port", 0).toUInt());
        if (settings->value("proxy_auth", false).toBool()) {
            proxy.setUser(settings->value("proxy_user", "").toString());
            proxy.setPassword(settings->value("proxy_pass", "").toString());
        }
    }
    else {
        proxy.setType(QNetworkProxy::NoProxy);
    }

    QNetworkProxy::setApplicationProxy(proxy);

    settings->endGroup();
}

void MainWindow::processCloseRequest(UI4chan* w, int reason) {
    int i;
    i = ui->tabWidget->indexOf((QWidget*)w);

    if (reason == 404) {
        if (settings->value("options/automatic_close", false).toBool()) {
            closeTab(i);
        }
    }
    else {
        closeTab(i);
    }
}

void MainWindow::replyFinished(QNetworkReply* r) {
    QString requestURI;
    QRegExp rx("Current version ([0-9\\.]+)", Qt::CaseInsensitive, QRegExp::RegExp2);
    QString html;
    QStringList res;
    int pos;

    if (r->isFinished()) {
        requestURI = r->request().url().toString();

        html = r->readAll();

        pos = rx.indexIn(html);
        res = rx.capturedTexts();

        if (pos != -1) {
           uiInfo->setCurrentVersion(res.at(1));
           checkVersion(res.at(1));
        }
    }

    r->deleteLater();
}


void MainWindow::updateWidgetSettings(void) {
    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UI4chan*)ui->tabWidget->widget(i))->updateSettings();
    }
}

void MainWindow::checkVersion(QString ver) {
    QStringList currVersion, thisVersion;

    currVersion = ver.split(".");
    thisVersion = QString(PROGRAM_VERSION).split(".");

    for (int i=0; i<currVersion.count(); i++) {
        if (currVersion.value(i).toInt() > thisVersion.at(i).toInt()) {
            newVersionAvailable(ver);
            break;
        }
    }
}

void MainWindow::newVersionAvailable(QString v) {
    QProcess process;
    QFileInfo fi;

    ui->statusBar->showMessage(QString("There is a new version (%1) available to download from sourceforge.").arg(v));
#ifdef USE_UPDATER
    switch (QMessageBox::question(0,"New version available",
                                  QString("There is a new version (%1) available from sourceforge<br><a href=\"http://sourceforge.net/projects/fourchan-dl/files/\">sourceforge.net/projects/fourchan-dl</a><br />Do you want to update now?").arg(v),
                                  QMessageBox::Yes | QMessageBox::No)) {
    case QMessageBox::Ok:
    case QMessageBox::Yes:

        fi.setFile(UPDATER_NAME);

        qDebug() << "Startet updater " << fi.absoluteFilePath();

        if (process.startDetached(QString("\"%1\"").arg(fi.absoluteFilePath()))) {
            ui->statusBar->showMessage("Startet updater");
        }
        else {
            ui->statusBar->showMessage("Unable to start process "+fi.absoluteFilePath()+" ("+process.errorString()+")");
        }
        aui->startUpdate(v);
        break;

    case QMessageBox::No:
    default:
        break;
    }
#else
    QMessageBox::information(0,
                             "New version available",
                             QString("There is a new version (%1) available from sourceforge<br><a href=\"http://sourceforge.net/projects/fourchan-dl/files/\">sourceforge.net/projects/fourchan-dl</a>").arg(v),
                             QMessageBox::Ok);
#endif
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
}

void MainWindow::startAll() {
    ui->pbOpenRequests->setFormat("Starting Thread %v/%m (%p%)");
    ui->pbOpenRequests->setMaximum(ui->tabWidget->count());

    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UI4chan*)ui->tabWidget->widget(i))->start();
        ui->pbOpenRequests->setValue((i+1));
    }
}

void MainWindow::stopAll() {
    ui->pbOpenRequests->setFormat("Stopping Thread %v/%m (%p%)");
    ui->pbOpenRequests->setMaximum(ui->tabWidget->count());

    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UI4chan*)ui->tabWidget->widget(i))->stop();
        ui->pbOpenRequests->setValue((i+1));
    }
}

void MainWindow::pendingThumbnailsChanged(int i) {
    if (i > ui->pbPendingThumbnails->maximum())
        ui->pbPendingThumbnails->setMaximum(i);

    if (i == 0) {
        ui->pbPendingThumbnails->setVisible(false);
        ui->pbPendingThumbnails->setMaximum(0);
    }
    else if (ui->pbPendingThumbnails->maximum() > 4)
        ui->pbPendingThumbnails->setVisible(true);

}

void MainWindow::addMultipleTabs() {
    threadAdder->show();
}
