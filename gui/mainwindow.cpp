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
    requestHandler = new RequestHandler(this);
    blackList = new BlackList(this);

    thumbnailRemoverThread = new QThread();
    thumbnailRemover = new ThumbnailRemover();
    thumbnailRemover->moveToThread(thumbnailRemoverThread);
    connect(thumbnailRemoverThread, SIGNAL(started()), thumbnailRemover, SLOT(removeOutdated()));
    thumbnailRemoverThread->start(QThread::LowPriority);

    overviewUpdateTimer = new QTimer(this);
    overviewUpdateTimer->setInterval(1000);
    overviewUpdateTimer->setSingleShot(true);
    _updateOverview = false;
    _paused = false;

    runUpdate = false;
    checkUpdaterVersion = false;

    connect(this, SIGNAL(removeFiles(QStringList)), thumbnailRemover, SLOT(removeFiles(QStringList)));
    ui->setupUi(this);

    // Adding actions to menu bar
    ui->menuBar->addAction(ui->actionAdd_Tab);
    ui->menuBar->addAction(ui->actionAddMultipleTabs);
    ui->menuBar->addAction(ui->actionTabOverview);

    ui->menuBar->addAction(ui->actionStart_all);
    ui->menuBar->addAction(ui->actionPauseAll);
    ui->menuBar->addAction(ui->actionStop_all);

    ui->menuBar->addAction(ui->actionOpen_Configuration);
//    ui->actionOpen_Configuration->setCheckable(true);

    historyMenu = new QMenu(ui->menuBar);
    historyMenu->setTitle("History");
    historyMenu->setIcon(QIcon(":/icons/resources/remove.png"));
    ui->menuBar->addMenu(historyMenu);

    ui->menuBar->addAction(ui->actionGetUpdaterVersion);

    ui->menuBar->addAction(ui->actionShowInfo);

    settings = new QSettings("settings.ini", QSettings::IniFormat);
    ui->tabWidget->removeTab(0);
    oldActiveTabIndex = 0;
    pendingThumbnailsChanged(0);

    // Thread overview
//    connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), ui->actionTabOverview, SLOT(setChecked(bool)));
    connect(ui->actionTabOverview, SIGNAL(triggered()), this, SLOT(scheduleOverviewUpdate()));

    loadOptions();
    restoreWindowSettings();
    updateWidgetSettings();

    connect(requestHandler, SIGNAL(response(QUrl,QByteArray,bool)), this, SLOT(processRequestResponse(QUrl,QByteArray,bool)));
    connect(requestHandler, SIGNAL(responseError(QUrl,int)), this, SLOT(handleRequestError(QUrl,int)));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(uiConfig, SIGNAL(configurationChanged()), this, SLOT(loadOptions()));
    connect(uiConfig, SIGNAL(configurationChanged()), blackList, SLOT(loadSettings()));
    connect(uiConfig, SIGNAL(configurationChanged()), downloadManager, SLOT(loadSettings()));
    connect(uiConfig, SIGNAL(deleteAllThumbnails()), thumbnailRemover, SLOT(removeAll()));
    connect(ui->actionStart_all, SIGNAL(triggered()), this, SLOT(startAll()));
    connect(ui->actionStop_all, SIGNAL(triggered()), this, SLOT(stopAll()));
    connect(ui->actionPauseAll, SIGNAL(triggered()), this, SLOT(pauseAll()));
    connect(threadAdder, SIGNAL(addTab(QString)), this, SLOT(createTab(QString)));
    connect(downloadManager, SIGNAL(error(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(downloadManager, SIGNAL(finishedRequestsChanged(int)), this, SLOT(updateDownloadProgress()));
    connect(downloadManager, SIGNAL(totalRequestsChanged(int)), this, SLOT(updateDownloadProgress()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(addThreadOverviewMark(int)));

    connect(overviewUpdateTimer, SIGNAL(timeout()), this, SLOT(overviewTimerTimeout()));
    connect(historyMenu, SIGNAL(triggered(QAction*)), this, SLOT(restoreFromHistory(QAction*)));

    connect(tnt, SIGNAL(pendingThumbnails(int)), ui->pbPendingThumbnails, SLOT(setValue(int)));
    connect(tnt, SIGNAL(pendingThumbnails(int)), this, SLOT(pendingThumbnailsChanged(int)));

    connect(aui, SIGNAL(connectionEstablished()), this, SLOT(updaterConnected()));
    connect(aui, SIGNAL(updateFinished()), this, SLOT(updateFinished()));
    connect(aui, SIGNAL(updaterVersionSent(QString)), this, SLOT(setUpdaterVersion(QString)));

//    createSupervisedDownload(QUrl("http://sourceforge.net/projects/fourchan-dl/files/"));
    createComponentList();

#ifdef __DEBUG__
    createSupervisedDownload(QUrl("file:d:/Qt/fourchan-dl/webupdate.xml"));
#else
    createSupervisedDownload(QUrl("http://www.sourceforge.net/projects/fourchan-dl/files/webupdate/webupdate.xml/download"));
#endif

#ifdef Q_OS_WIN
    win7.init((HWND)this->winId());
#endif

    createTrayIcon();

    autosaveTimer = new QTimer(this);
    autosaveTimer->setInterval(1000*60*10);     // 10 Minutes
    autosaveTimer->setSingleShot(false);
    connect(autosaveTimer, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::addTab() {
    int ci;
    UIImageOverview* w;

    w = new UIImageOverview(this);
    w->setBlackList(blackList);

    ci = ui->tabWidget->addTab(w, "no name");
    if (settings->value("options/remember_directory", false).toBool())
        w->setDirectory(defaultDirectory);

    connect(w, SIGNAL(errorMessage(QString)), this, SLOT(displayError(QString)));
    connect(w, SIGNAL(tabTitleChanged(UIImageOverview*, QString)), this, SLOT(changeTabTitle(UIImageOverview*, QString)));
    connect(w, SIGNAL(closeRequest(UIImageOverview*, int)), this, SLOT(processCloseRequest(UIImageOverview*, int)));
    connect(w, SIGNAL(directoryChanged(QString)), this, SLOT(setDefaultDirectory(QString)));
    connect(w, SIGNAL(createTabRequest(QString)), this, SLOT(createTab(QString)));
    connect(w, SIGNAL(removeFiles(QStringList)), this, SIGNAL(removeFiles(QStringList)));
    connect(w, SIGNAL(changed()), this, SLOT(scheduleOverviewUpdate()));

    changeTabTitle(w, "idle");

    return ci;
}

int MainWindow::addForegroundTab() {
    int ci;
    UIImageOverview* w;
    QStringList sl;

    if (threadExists(QApplication::clipboard()->text())) {
        ci = addTab();
        w = ((UIImageOverview*)ui->tabWidget->widget(ci));
        sl = w->getValues().split(";;");
        sl.replace(0, "");
        w->setValues(sl.join(";;"));
    }
    else {
        ci = addTab();
    }
    ui->tabWidget->setCurrentIndex(ci);

    return ci;
}

void MainWindow::createTab(QString s) {
    int index;
    UIImageOverview* w;
    QStringList sl;

    sl = s.split(";;");

    if (!threadExists(sl.at(0))) {
        index = addTab();
        w = ((UIImageOverview*)ui->tabWidget->widget(index));
        w->setValues(s);
        w->setAttribute(Qt::WA_DeleteOnClose, true);
    }
    else {
        QLOG_INFO() << "MainWindow :: Prevented opening of thread" << sl.at(0) << "because it's already open.";
    }
}

void MainWindow::closeTab(int i) {
    UIImageOverview* w;

    ui->tabWidget->setCurrentIndex(i);
    w = (UIImageOverview*)ui->tabWidget->widget(i);

    addToHistory(w->getValues(), w->getTitle());

    if (w->close()) {
        ui->tabWidget->removeTab(i);
        w->deleteLater();
    }
    else {
        QLOG_WARN() << "MainWindow :: Close widget event not accepted";
    }

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

void MainWindow::changeTabTitle(UIImageOverview* w, QString s) {
    int i;

    i = ui->tabWidget->indexOf((QWidget*)w);
    ui->tabWidget->setTabText(i, s);
}

void MainWindow::restoreWindowSettings(void) {
    // Restore window position
    QPoint p;
    QSize s;
    QByteArray ba;
    int state;

    settings->beginGroup("window");
        p = settings->value("position",QPoint(0,0)).toPoint();
        state = settings->value("state",0).toInt();
        s = settings->value("size",QSize(0,0)).toSize();
        ba = settings->value("widgetstate", QByteArray()).toByteArray();
    settings->endGroup();

    if (p != QPoint(0,0))
        this->move(p);

    if (s != QSize(0,0))
        this->resize(s);

    if (state != Qt::WindowNoState)
        this->setWindowState((Qt::WindowState) state);

    if (!ba.isEmpty())
        this->restoreState(ba);

//    ui->threadOverview->setVisible(settings->value("thread_overview/visible", true).toBool());
    ui->actionTabOverview->setChecked(settings->value("thread_overview/visible", true).toBool());
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

            ((UIImageOverview*)ui->tabWidget->widget(ci))->setValues(
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

    QLOG_INFO() << "MainWindow :: Saving settings";
    // Window related stuff
    settings->beginGroup("window");
        settings->setValue("position", this->pos());
        if (this->windowState() == Qt::WindowNoState)
            settings->setValue("size", this->size());
        settings->setValue("state", QString("%1").arg(this->windowState()));
        settings->setValue("widgetstate", this->saveState());
    settings->endGroup();

    // Dock widget
    settings->beginGroup("thread_overview");
//    settings->setValue("size", ui->dockWidget->size());
    settings->setValue("col_uri_width", ui->threadOverview->columnWidth(3));
    settings->setValue("col_name_width", ui->threadOverview->columnWidth(0));
    settings->setValue("col_images_width", ui->threadOverview->columnWidth(1));
    settings->setValue("col_status_width", ui->threadOverview->columnWidth(2));
    settings->setValue("visible", ui->threadOverview->isVisible());
    settings->endGroup();

    // Options
    settings->beginGroup("options");
    settings->endGroup();

    // Active tabs
    settings->remove("tabs");   // Clean up
    settings->sync();

    settings->beginGroup("tabs");
        settings->setValue("count", ui->tabWidget->count());

        for (int i=0; i<ui->tabWidget->count(); i++) {
            settings->setValue(QString("tab%1").arg(i), ((UIImageOverview*)ui->tabWidget->widget(i))->getValues());
        }
    settings->endGroup();

    downloadManager->getStatistics(&downloadedFiles, &downloadedKB);
    settings->beginGroup("statistics");
        settings->setValue("downloaded_files", downloadedFiles);
        settings->setValue("downloaded_kbytes", downloadedKB);
    settings->endGroup();

    settings->sync();

    if (settings->status() == QSettings::NoError) {
        QLOG_INFO() << "MainWindow :: Settings saved successfully";
    }
    else {
        QLOG_ERROR() << "MainWindow :: Saving settings failed";
        QLOG_ERROR() << "MainWindow ::  error: " << settings->status();
    }
    imageViewer->saveSettings();
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

    // Dock widget
    settings->beginGroup("thread_overview");
//    ui->dockWidget->resize();
//    settings->setValue("width", ui->dockWidget->width());
    ui->threadOverview->setColumnWidth(3, settings->value("col_uri_width", 170).toInt());
    ui->threadOverview->setColumnWidth(0, settings->value("col_name_width", 190).toInt());
    ui->threadOverview->setColumnWidth(1, settings->value("col_images_width", 60).toInt());
    ui->threadOverview->setColumnWidth(2, settings->value("col_status_width", 70).toInt());
    settings->endGroup();
}

void MainWindow::processCloseRequest(UIImageOverview* w, int reason) {
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

void MainWindow::processRequestResponse(QUrl url, QByteArray ba, bool cached) {

    if (url.toString().contains("webupdate.xml")) {
        checkForUpdates(QString(ba));
    }
    else {
        QLOG_WARN() << "MainWindow :: MainWindow should only ask for webupdate.xml but response was for" << url.toString();
    }
}

void MainWindow::updateWidgetSettings(void) {
    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UIImageOverview*)ui->tabWidget->widget(i))->updateSettings();
    }
}

void MainWindow::newComponentsAvailable() {
#ifdef USE_UPDATER
    QProcess process;
    QFileInfo fi;
#endif

    QString msg;

    msg = "There are new components available to download from sourceforge:";

    for (int i=0; i<updateableComponents.count(); i++) {
        component_information c;

        c = components.value(updateableComponents.at(i));

        msg.append("<br>");
        msg.append(QString("&nbsp;&nbsp;%1:%2 (installed: %3, available: %4)").arg(c.type).arg(c.componentName).arg(c.version).arg(c.remote_version));
    }
//    ui->statusBar->showMessage(msg);

#ifdef USE_UPDATER
    msg.append("<br>Do you want to update now?");

    switch (QMessageBox::question(0,"New version available",
                                  msg,
                                  QMessageBox::Yes | QMessageBox::No)) {
    case QMessageBox::Ok:
    case QMessageBox::Yes:

        fi.setFile(updaterFileName);

        QLOG_INFO() << "MainWindow :: Starting updater " << fi.absoluteFilePath();

        if (process.startDetached(QString("\"%1\"").arg(fi.absoluteFilePath()))) {
            ui->statusBar->showMessage("Starting updater");
        }
        else {
            ui->statusBar->showMessage("Unable to start process "+fi.absoluteFilePath()+" ("+process.errorString()+")");
        }
        break;

    case QMessageBox::No:
    default:
        break;
    }
#else
    msg.append("<br><a href=\"http://sourceforge.net/projects/fourchan-dl/files/\">http://sourceforge.net/projects/fourchan-dl</a>");

    QMessageBox::information(0,
                             "New version available",
                             msg,
                             QMessageBox::Ok);
#endif
}

void MainWindow::getUpdaterVersion() {
#ifdef USE_UPDATER
    QProcess process;
    QFileInfo fi;

    fi.setFile(updaterFileName);

    QLOG_INFO() << "MainWindow :: Starting updater " << fi.absoluteFilePath();

    checkUpdaterVersion = true;
    if (process.startDetached(QString("\"%1\"").arg(fi.absoluteFilePath()))) {
        ui->statusBar->showMessage("Starting updater for version check");
    }
    else {
        ui->statusBar->showMessage("Unable to start process "+fi.absoluteFilePath()+" ("+process.errorString()+")");
        checkUpdaterVersion = false;
    }

#endif
}

void MainWindow::startAll() {
    ui->pbOpenRequests->setFormat("Starting Thread %v/%m (%p%)");
    ui->pbOpenRequests->setMaximum(ui->tabWidget->count());

    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UIImageOverview*)ui->tabWidget->widget(i))->start();
        ui->pbOpenRequests->setValue((i+1));
    }
}

void MainWindow::pauseAll() {
    if (_paused) {
        ui->actionPauseAll->setIcon(QIcon(":/icons/resources/media-playback-pause.png"));
        downloadManager->resumeDownloads();
        tnt->resume();
    }
    else {
        ui->actionPauseAll->setIcon(QIcon(":/icons/resources/media-playback-pause-red.png"));
        downloadManager->pauseDownloads();
        tnt->halt();
    }

    _paused = !_paused;
}

void MainWindow::stopAll() {
    ui->pbOpenRequests->setFormat("Stopping Thread %v/%m (%p%)");
    ui->pbOpenRequests->setMaximum(ui->tabWidget->count());

    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UIImageOverview*)ui->tabWidget->widget(i))->stop();
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

void MainWindow::showTab(QTreeWidgetItem* item, int idx) {
    int index;

    index = ui->threadOverview->indexOfTopLevelItem(item);

    if (index != -1) {
        ui->tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::scheduleOverviewUpdate() {
    if (!overviewUpdateTimer->isActive()) {
        updateThreadOverview();
        overviewUpdateTimer->start();
    }
    else {
         _updateOverview = true;
    }
}

void MainWindow::overviewTimerTimeout() {
    updateThreadOverview();
    if (_updateOverview) {
        _updateOverview = false;
        overviewUpdateTimer->start();
    }
}

void MainWindow::updateThreadOverview() {
//    QList<QTreeWidgetItem *> items;
    QStringList sl;

    if (ui->threadOverview->isVisible()) {
//        QLOG_TRACE() << "MainWindow :: updating thread overview";
//        ui->threadOverview->clear();

        for (int i=0; i<ui->tabWidget->count(); i++) {
            UIImageOverview* tab;
            QTreeWidgetItem* item;
            sl.clear();

            tab = (UIImageOverview*)(ui->tabWidget->widget(i));
            sl << tab->getTitle();
            sl << QString("%1/%2").arg(tab->getDownloadedImagesCount()).arg(tab->getTotalImagesCount());
            sl << tab->getStatus();
            sl << tab->getURI();

            if (ui->threadOverview->topLevelItemCount() > i) {                   // If there is an entry for the i-th tab
                item = ui->threadOverview->topLevelItem(i);     //  change its content
                for (int k=0; k<4; k++)
                    item->setText(k, sl.at(k));
            }
            else {                                              // Otherwise create a new one and append it
                ui->threadOverview->addTopLevelItem(new QTreeWidgetItem(ui->threadOverview, sl));
            }
        }

        // Remove obsolete rows of overview (if any)
        if (ui->threadOverview->topLevelItemCount() > ui->tabWidget->count()) {
            for (int i=ui->threadOverview->topLevelItemCount(); i>=ui->tabWidget->count(); --i) {
                ui->threadOverview->takeTopLevelItem(i);
            }
        }
//        ui->threadOverview->insertTopLevelItems(0, items);
    }
}

void MainWindow::debugButton() {
    updateThreadOverview();
}

bool MainWindow::threadExists(QString url) {
    bool ret;
    int count;
    QString widgetUri;

    ret = false;
    count = 0;

    for (int i=0; i<ui->tabWidget->count(); i++) {
        widgetUri = ((UIImageOverview*)ui->tabWidget->widget(i))->getURI();

        if ( widgetUri.left(widgetUri.lastIndexOf("#")) == url.left(url.lastIndexOf("#"))) {
            count++;
        }
    }

    if (count > 0)
        ret = true;

    return ret;
}

void MainWindow::addToHistory(QString s, QString title="") {
    QStringList sl;
    QString key;
    QString actionTitle;
    QAction* a;

    sl = s.split(";;");
    if (sl.count() > 0) {
        key = sl.at(0);
        if (!key.isEmpty()) {
            historyList.insert(key, s);

            if (title.isEmpty())
                actionTitle = QString("%1 -> %2").arg(key).arg(sl.at(1));
            else
                actionTitle = QString("%1 (%2) -> %3").arg(key).arg(title).arg(sl.at(1));

            a = historyMenu->addAction(actionTitle);
            a->setIcon(QIcon(":/icons/resources/reload.png"));
            a->setStatusTip(QString("Reopen thread %1").arg(key));
            a->setToolTip(key); // Abusing the tooltip for saving the historyList key
            QLOG_TRACE() << "MainWindow :: " << QString("Adding (%1, %2) to history").arg(key).arg(s);
        }
    }
}

void MainWindow::removeFromHistory(QString key) {
    historyList.remove(key);
    QLOG_TRACE() << "MainWindow :: " << QString("Removing (%1) from history").arg(key);
}

void MainWindow::restoreFromHistory(QAction* a) {
    QString key;

    key = a->toolTip();

    if (historyList.count(key) > 0)
        createTab(historyList.value(key, ""));

    removeFromHistory(key);
    historyMenu->removeAction(a);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QMainWindow::keyPressEvent(event);

    if (event->key() == Qt::Key_W && (event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
        closeTab(ui->tabWidget->currentIndex());
    }
}

void MainWindow::createSupervisedDownload(QUrl url) {
    if (url.isValid()) {
        requestHandler->request(url, 0);
    }
}

void MainWindow::removeSupervisedDownload(QUrl url) {
    requestHandler->cancel(url);
}

void MainWindow::checkForUpdates(QString xml) {
    QRegExp rx(QString("<%1>([\\w\\W]+[^<])+</%1>").arg(UPDATE_TREE), Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxFile("<file name=\"([^\\\"]+)\" filename=\"([^\\\"]+)\" type=\"([^\\\"]+)\" version=\"([\\w\\.]*)\" source=\"([\\w:\\-\\./\\+]+)\" target=\"([^\\\"]+)\" />", Qt::CaseInsensitive, QRegExp::RegExp2);
    int pos, posFile;
    QStringList res, resFile;
    QMap<QString, component_information> comp;
    component_information c, local, remote;
    QList<QString> foundComponents;

    pos = rx.indexIn(xml);
    res = rx.capturedTexts();

    if (res.count() > 0 && pos != -1) {

        posFile = 0;
        while (posFile != -1) {
            posFile = rxFile.indexIn(res.at(1), posFile+1);
            resFile = rxFile.capturedTexts();

            if (resFile.at(1) != "") {

                c.componentName = resFile.at(1);
                c.filename = resFile.at(2);
                c.type = resFile.at(3);
                c.version = resFile.at(4);
                c.src = resFile.at(5);
                c.target = resFile.at(6);

                if (c.filename == APP_NAME && c.type == "executable") {
                    uiInfo->setCurrentVersion(c.version);
                }

                comp.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
            }
        }
    }

    foundComponents = comp.keys();

    foreach (QString key, foundComponents) {
        local = components.value(key);
        remote = comp.value(key);

        if (local.filename == remote.filename) {
            if (checkIfNewerVersion(remote.version, local.version)) {
                QLOG_INFO() << "MainWindow :: New version available for " << local.type << ":" << local.filename;
                updateableComponents.append(key);
                local.src = remote.src;
                local.target = remote.target;
                local.remote_version = remote.version;
                components.insert(key,local);
                runUpdate = true;
            }
        }
        else if (remote.filename != "" && local.filename == "") {
            // New component!
            remote.remote_version = remote.version;
            remote.version = "no";
            components.insert(key, remote);
            runUpdate = true;
            updateableComponents.append(key);
        }
    }

    if (runUpdate) newComponentsAvailable();
    QLOG_TRACE() << "MainWindow :: " << xml;
}

bool MainWindow::checkIfNewerVersion(QString _new, QString _old) {
    bool ret;
    QStringList newVersion, oldVersion;

    ret = false;

    newVersion = _new.split(".");
    oldVersion = _old.split(".");

    for (int i=0; i<newVersion.count(); i++) {
        if (newVersion.value(i).toInt() > oldVersion.at(i).toInt()) {
            ret = true;
            break;
        }
        else {
            if (oldVersion.at(i).toInt() > newVersion.value(i).toInt()) {
                break;
            }
        }
    }

    return ret;
}

void MainWindow::createComponentList() {
    component_information c;
    QStringList plugins;
    QStringList qtFiles;
    QStringList neededLibraries;

#ifdef USE_UPDATER
    QString version;
#endif

#ifdef Q_OS_WIN32
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qtFiles << "Qt5Core" << "Qt5Gui" << "Qt5Widgets" << "Qt5Network" << "Qt5Xml";
    neededLibraries << "libeay32.dll" << "ssleay32.dll" << "libstdc++-6.dll" << "imageformats/qgif.dll"
                    << "imageformats/qico.dll" << "imageformats/qjpeg.dll" << "imageformats/qsvg.dll"
                    << "libgcc_s_sjlj-1.dll" << "libwinpthread-1.dll";

#else
    qtFiles << "QtCore4" << "QtGui4" << "QtNetwork4" << "QtXml4";
    neededLibraries << "libeay32.dll" << "ssleay32.dll" << "imageformats/qgif4.dll"
                    << "imageformats/qico4.dll" << "imageformats/qjpeg4.dll"
                    << "imageformats/qmng4.dll" << "imageformats/qsvg4.dll"
                    << "imageformats/qtiff4.dll";
#endif
#endif

    components.clear();

    c.filename = APP_NAME;
    c.componentName = "Main program";
    c.type = "executable";
    c.version = PROGRAM_VERSION;

    components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);

    foreach (QString libFile, neededLibraries) {
        c.filename = libFile;
        c.componentName = libFile;
        c.type = "library";
        c.version = "";
        QLOG_ALWAYS() << "Mainwidow :: createComponentList :: Checking for " << QString("%1/%2").arg(QApplication::applicationDirPath()).arg(libFile);
        if (QFile::exists(QString("%1/%2").arg(QApplication::applicationDirPath()).arg(libFile))) {
            components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
        }
        else {
            QLOG_WARN() << "Mainwidow :: createComponentList :: Needed library " << libFile << "does not exist.";
        }
    }

    if (QFile::exists(QString("%1/%2").arg(QApplication::applicationDirPath()).arg(CONSOLE_APPNAME))) {
        c.filename = CONSOLE_APPNAME;
        c.componentName = "Console";
        c.type = "executable";
        c.version = settings->value("console/version", "0.1.0").toString();
        components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
    }

#ifdef USE_UPDATER
    // TODO: use updaterFileName (but without the path)
    c.filename = "upd4t3r.exe";
    c.componentName = "Updater";
    c.type = "executable";
    version = settings->value("updater/version", "unknown").toString();

    if (version == "unknown" && c.filename.contains("upd4t3r")) {
        // No version information in settings file, but new updater executable present
        // means a freshly updated system. Assume version 1.1
        version = "1.1";
        settings->setValue("updater/version", version);
    }

    c.version = version;


    components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
#endif

    foreach (QString f, qtFiles) {
#ifdef Q_OS_WIN32
        c.filename = QString("%1.dll").arg(f);
#else
#ifdef Q_OS_LINUX
        c.filename = f;
#endif
#endif
        c.componentName = f;
        c.type = "qt";
        c.version = qVersion();

        components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
    }

    plugins = pluginManager->getAvailablePlugins();

    foreach (QString pluginName, plugins) {
        c = pluginManager->getInfo(pluginName);
        components.insert(QString("%1:%2").arg(c.type).arg(c.filename), c);
    }

    uiInfo->loadComponentInfo(components);
}

void MainWindow::updaterConnected() {
    // We requested the update executable to start and now we have a connection
    QStringList fileList;
    component_information c;

    if (checkUpdaterVersion) {
        QLOG_INFO() << "Mainwidow :: updaterConnected :: Just checking updater version";
        aui->closeUpdaterExe();
        checkUpdaterVersion = false;
    }
    else {
        if (runUpdate) {
            tnt->halt();
            downloadManager->pauseDownloads();

            foreach (QString component, updateableComponents) {
                c = components.value(component);
                fileList.append(QString("%1->%2").arg(c.src).arg(c.target));
            }
            aui->addFiles(fileList);
            aui->startUpdate();
        }
        else {
            QLOG_WARN() << "Mainwidow :: updaterConnected :: Updater connected, but I didn't know what to do";
            aui->closeUpdaterExe();
        }
    }
}

void MainWindow::updateFinished() {
    aui->closeUpdaterExe();
    aui->exchangeFiles();
}

QList<component_information> MainWindow::getComponents() {
    QList<component_information> ret;
    QStringList keys;

    keys = components.keys();

    foreach(QString key, keys) {
        ret.append(components.value(key));
    }

    return ret;
}

void MainWindow::setUpdaterVersion(QString v) {
    settings->setValue("updater/version", v);
}

void MainWindow::updateDownloadProgress() {
#ifdef Q_OS_WIN
    int total, finished;

    total = downloadManager->getTotalRequests();
    finished = downloadManager->getFinishedRequests();

    if (finished != total || total != 0) {
        win7.setProgressValue(finished, total);
        win7.setProgressState(win7.Normal);
    }
    else {
        win7.setProgressState(win7.NoProgress);
    }

#endif
}

#ifdef Q_OS_WIN
bool MainWindow::winEvent(MSG *message, long *result)
{
    return win7.winEvent(message, result);
}
#endif

void MainWindow::createTrayActions()
{
    restoreAction = new QAction(tr("&Restore"), this);
    restoreAction->setIcon(QIcon(":/icons/resources/rotateCW.png"));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setIcon(QIcon(":/icons/resources/close.png"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);

    if (QSystemTrayIcon::isSystemTrayAvailable() && settings->value("options/close_to_tray", false).toBool()) {
        createTrayActions();

        trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(restoreAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        trayIcon->setContextMenu(trayIconMenu);

        QApplication::setQuitOnLastWindowClosed(false);

        trayIcon->setIcon(QIcon(":/icons/resources/4chan.ico"));
        trayIcon->show();

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }
}

void MainWindow::removeTrayIcon() {
    if (QSystemTrayIcon::isSystemTrayAvailable() && settings->value("options/close_to_tray", false).toBool() && trayIcon) {
        trayIcon->hide();
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason ar) {
    if (ar == QSystemTrayIcon::DoubleClick) {
        restoreAction->trigger();
    }
}

void MainWindow::toggleThreadOverview() {
    if (ui->dockWidget->isVisible()) {
        ui->dockWidget->setVisible(false);
    }
    else {
        ui->dockWidget->setVisible(true);
    }
}

void MainWindow::aboutToQuit() {
    downloadManager->pauseDownloads();
    tnt->halt();
    tnt->deleteLater();
    saveSettings();
    removeTrayIcon();
    cleanThreadCache();
    thumbnailRemoverThread->terminate();

    emit quitAll();
}

void MainWindow::removeThreadOverviewMark() {
    for (int i=0; i<ui->threadOverview->topLevelItemCount(); i++) {
        ui->threadOverview->topLevelItem(i)->setIcon(0, QIcon());
    }
}

void MainWindow::addThreadOverviewMark(QTreeWidgetItem* item) {
    removeThreadOverviewMark();

    item->setIcon(0, QIcon(":/icons/resources/go-next.png"));
}

void MainWindow::addThreadOverviewMark(int index) {
    QList<QTreeWidgetItem*> foundItems;

    foundItems = ui->threadOverview->findItems(((UIImageOverview*)(ui->tabWidget->widget(index)))->getURI(), Qt::MatchExactly, 3 );
    if (foundItems.count() == 1) {
        addThreadOverviewMark(foundItems.at(0));
    }
}

void MainWindow::cleanThreadCache() {
    QStringList threadCachesToRemove;
    QStringList dirContents;
    QString cacheFolder;
    QString url;
    QString cacheFile;
    QDir dir;

    if (settings->value("download_manager/use_thread_cache", false).toBool()) {
        cacheFolder = settings->value("download_manager/thread_cache_path", "").toString();

        if (!cacheFolder.isEmpty()) {
            dir.setPath(cacheFolder);
            if (dir.isReadable()) {
                dirContents = dir.entryList(QStringList() << "*.tcache");
            }
        }

        foreach (cacheFile, dirContents) {
            threadCachesToRemove << QString("%1/%2").arg(cacheFolder, cacheFile);
        }

        for (int i=0; i<ui->tabWidget->count(); i++) {
            url = ((UIImageOverview*)ui->tabWidget->widget(i))->getURI();
            cacheFile = downloadManager->getFilenameForURL(QUrl(url));
            threadCachesToRemove.removeAll(cacheFile);
        }

        foreach (cacheFile, threadCachesToRemove) {
            QFile::remove(cacheFile);
        }
    }
}
