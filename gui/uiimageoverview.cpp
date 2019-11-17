﻿#include "uiimageoverview.h"
#include "ui_uiimageoverview.h"

UIImageOverview::UIImageOverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIImageOverview)
{
    QClipboard *clipboard = QApplication::clipboard();
    QTime time;

    requestHandler = new RequestHandler(this);
    iParser = 0;
    oParser = 0;
    parserThread = new QThread();
    _cachedResult = false;
    _threadBlocked = false;
    fresh_thread = true;
    follow_redirects = true;

    pendingThumbnails.clear();

    ui->setupUi(this);

#ifndef __DEBUG__
    ui->btnDebug->hide();
#endif

    timer = new QTimer(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    thumbnailCheckTimer = new QTimer(this);
    thumbnailCheckTimer->setInterval(5000+time.msec());
    thumbnailCheckTimer->setSingleShot(true);

    thumbnailsizeLocked = false;
    closeWhenFinished = false;
    downloading = false;

    deleteFileAction = new QAction(QString("Delete File"), this);
    deleteFileAction->setIcon(QIcon(":/icons/resources/remove.png"));
    reloadFileAction = new QAction("Reload File", this);
    reloadFileAction->setIcon(QIcon(":/icons/resources/reload.png"));
    openFileAction = new QAction("Open File", this);
    openFileAction->setIcon(QIcon(":/icons/resources/open.png"));

    ui->progressBar->setMinimum(1);
    ui->progressBar->setMaximum(-1);
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(true);
    ui->progressBar->setVisible(false);
    ui->progressBar->setFormat("%p%");

    if (QUrl(clipboard->text()).isValid() && pluginManager->isSupported(clipboard->text())) {
        ui->leURI->setText(clipboard->text());
    }

    connect(requestHandler, SIGNAL(responseError(QUrl, int)), this, SLOT(errorHandler(QUrl, int)));
    connect(requestHandler, SIGNAL(response(QUrl, QByteArray, bool)), this, SLOT(processRequestResponse(QUrl, QByteArray, bool)));

    connect(ui->leSavepath, SIGNAL(textChanged(QString)), this, SIGNAL(directoryChanged(QString)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openFile()));

    connect(deleteFileAction, SIGNAL(triggered()), this, SLOT(deleteFile()));
    connect(reloadFileAction, SIGNAL(triggered()), this, SLOT(reloadFile()));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));

    connect(timer, SIGNAL(timeout()), this, SLOT(triggerRescan()));
    connect(folderShortcuts, SIGNAL(shortcutsChanged()), this, SLOT(fillShortcutComboBox()));
    connect(ui->cbFolderShortcuts, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectShortcut(QString)));
    connect(ui->leSavepath, SIGNAL(textChanged(QString)), this, SLOT(checkForFolderShortcut(QString)));
    connect(ui->btnAddShortcut, SIGNAL(clicked()), this, SLOT(addShortcut()));

    // Connections for key bindings
    connect(ui->listWidget, SIGNAL(openItem()), this, SLOT(openFile()));
    connect(ui->listWidget, SIGNAL(deleteItem()), this, SLOT(deleteFile()));
    connect(ui->listWidget, SIGNAL(reloadItem()), this, SLOT(reloadFile()));

    connect(tnt, SIGNAL(thumbnailAvailable(QString,QString)), this, SLOT(addThumbnail(QString,QString)));
    connect(tnt, SIGNAL(thumbnailsAvailable(QString)), this, SLOT(addThumbnails(QString)));

    connect(thumbnailCheckTimer, SIGNAL(timeout()), this, SLOT(checkForMissingThumbnails()));

    setTabTitle("idle");

    loadSettings();
    fillShortcutComboBox();
    expectedThumbnailCount = 0;
    thumbnailCount = 0;
    thumbnailCountLastViewed = 0;
}

UIImageOverview::~UIImageOverview()
{
    delete ui;
}

bool UIImageOverview::setThumbnailSize(QSize s) {
    bool ret;

    ret = false;

    if (!thumbnailsizeLocked) {
        iconSize = s;
        ui->listWidget->setIconSize(iconSize);
        ui->listWidget->setGridSize(QSize(iconSize.width()+10,iconSize.height()+20));
        thumbnailsizeLocked = true;
    }

    return ret;
}

void UIImageOverview::start(void) {
    QDir dir;
    QString savepath;

    if (_threadBlocked) {
        setStatus("Blocked");
    }
    else {
        running = true;
        setStatus("Running");

        if (ui->leURI->text() != "") {
            ui->leURI->setReadOnly(true);

            if (!ui->leURI->text().startsWith("http")) {
                QString s;
                s = ui->leURI->text();
                s.prepend("http://");
                ui->leURI->setText(s);
            }

            // Check if we can parse this URI
            if (selectParser()) {
                savepath = getSavepath();

                if (savepath.endsWith("\\")) {
                    savepath.chop(1);
                    //ui->leSavepath->setText(savepath);
                }
                QLOG_TRACE() << "UIImageOverview :: Setting save path to " << savepath;
                dir.setPath(savepath);

                if (!dir.exists()) {
                    QDir d;

                    d.mkpath(savepath);
                    QLOG_INFO() << "UIImageOverview :: Directory" << savepath << " does not exist. Creating...";
                }

                if (dir.exists()) {
                    ui->leSavepath->setEnabled(false);
                    startDownload();

                    ui->btnStart->setEnabled(false);
                    ui->btnStop->setEnabled(true);
                    ui->cbRescan->setEnabled(false);
                    ui->comboBox->setEnabled(false);
                    //                ui->progressBar->setEnabled(true);
                    ui->cbOriginalFilename->setEnabled(false);
                    ui->btnChoosePath->setEnabled(false);
                    ui->cbFolderShortcuts->setEnabled(false);

                    if (ui->cbRescan->isChecked()) {
                        timer->setInterval(timeoutValues.at(ui->comboBox->currentIndex())*1000);

                        timer->start();
                    }
                    // Hide thread settings
                    if ((ui->btnToggleView->isChecked()))
                        ui->btnToggleView->setChecked(false);
                }
                else
                {
                    stop();
                    emit errorMessage("Directory does not exist / Could not be created");
                    setStatus("Directory does not exist / Could not be created");
                    QLOG_ERROR() << "UIImageOverview :: Directory" << savepath << " does not exist and I couldn't create it.";
                }
            }
            else {
                stop();
                emit errorMessage("Could not find a parser for this URL (" + ui->leURI->text() + ")");
                setStatus("No parser available");
                QLOG_WARN() << "UIImageOverview :: I couldn't find a parser for uri " << ui->leURI->text();
            }
        }
    }
}

void UIImageOverview::stop(void) {
    running = false;
    stopDownload();
    timer->stop();
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->leURI->setReadOnly(false);
    ui->leSavepath->setEnabled(true);
    ui->cbRescan->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->progressBar->setVisible(false);
    ui->cbOriginalFilename->setEnabled(true);
    ui->btnChoosePath->setEnabled(true);
    ui->cbFolderShortcuts->setEnabled(true);

    // Bring up thread settings
    if (!(ui->btnToggleView->isChecked()))
        ui->btnToggleView->setChecked(true);

    if (oParser != 0) {
        oParser->deleteLater();
        oParser = 0;
        iParser = 0;
    }

    setStatus("Stopped");
}

void UIImageOverview::chooseLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose storage directory", ui->leSavepath->text());

    if (loc != "") {
        if (loc.endsWith("\\"))
            loc.chop(1);
        ui->leSavepath->setText(loc);
        emit directoryChanged(loc);
    }
}

void UIImageOverview::triggerRescan(void) {
    if (!_threadBlocked) {
        startDownload();
        timer->start();

        setStatus("rescanning");
    }
}

void UIImageOverview::createThumbnail(QString s) {
    QString thumbnail_location;
    if (!_threadBlocked) {
        thumbnail_location = tnt->addToList(s);
        QLOG_TRACE() << Q_FUNC_INFO << ":: Adding thumbnail for" << s;
        pendingThumbnails.append(s);

        missingThumbs.insert(s, thumbnail_location);
        if (!thumbnailCheckTimer->isActive()) {
            thumbnailCheckTimer->start();
        }
        expectedThumbnailCount++;
    }
}

void UIImageOverview::addThumbnail(QString filename, QString tnFilename) {
    QListWidgetItem* item;

    QLOG_TRACE() << Q_FUNC_INFO << "::" << filename << "," << tnFilename;
    if (tnFilename != "") {
//        if (pendingThumbnails.contains(filename)) {
//            pendingThumbnails.removeAll(filename);
        if (missingThumbs.contains(filename)) {
//            missingThumbs.remove(filename);
            item = new QListWidgetItem(
                        QIcon(tnFilename),
                        filename,
                        ui->listWidget);

            ui->listWidget->addItem(item);
            thumbnailsizeLocked = true;

            if (++thumbnailCount >= expectedThumbnailCount) {
                //    if (isDownloadFinished()) {
                updateDownloadStatus();
            }

            if (this->isVisible()) {
                thumbnailCountLastViewed = thumbnailCount;
            }
        }
    }
}

void UIImageOverview::addThumbnails(QString thumbnails) {
    QStringList image_filenames;
    image_filenames = thumbnails.split(";;;");

    QLOG_TRACE() << Q_FUNC_INFO << ":: Map size " << image_filenames.count();
    for (int i=0; i<image_filenames.count(); i++) {
        if (pendingThumbnails.contains(image_filenames.at(i).split(":::").at(0))) {
            addThumbnail(image_filenames.at(i).split(":::").at(0),
                         image_filenames.at(i).split(":::").at(1));
        }
    }
}

void UIImageOverview::checkForMissingThumbnails() {
    QMap<QString,QString>::iterator i;
    QList<QString> found_thumbs;

    QLOG_TRACE() << Q_FUNC_INFO << ":: Checking for thumbnails";

    for (i=missingThumbs.begin(); i!=missingThumbs.end(); i++) {
        QLOG_TRACE() << Q_FUNC_INFO << ":: Checking for thumbnail" << i.value();
        if (QFile::exists(i.value())) {
            QLOG_TRACE() << Q_FUNC_INFO << ":: " << i.value() << "found";
            addThumbnail(i.key(), i.value());
            found_thumbs << i.key();
        }
    }

    for (int k=0; k<found_thumbs.count(); k++) {
        missingThumbs.remove(found_thumbs.at(k));
    }

    if (missingThumbs.size() > 0) {
        thumbnailCheckTimer->start();
    }
    else {
        updateDownloadStatus();
    }
}

void UIImageOverview::on_listWidget_customContextMenuRequested(QPoint pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    contextMenu.addAction(openFileAction);
    contextMenu.addAction(deleteFileAction);
    contextMenu.addAction(reloadFileAction);
    contextMenu.exec(ui->listWidget->mapTo(this, mapToGlobal(pos)));
}

void UIImageOverview::deleteFile(void) {
    QFile f;
    QString filename;
    QString uri;
    QString cacheFile;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        f.setFileName(filename);

        if (f.exists()) {
            f.remove();

            ui->listWidget->takeItem(ui->listWidget->currentRow());
        }

        if (getUrlOfFilename(filename, &uri))
            blackList->add(uri);
    }

    cacheFile = tnt->getCacheFile(filename);

    emit removeFiles(QStringList(cacheFile));
}

void UIImageOverview::reloadFile(void) {
    QString filename;
    QFile f;
    int i;
    _IMAGE tmp;
    bool checkOrgFilename;
    bool checkLargeURI;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        f.setFileName(filename);

        if (f.exists()) {
            f.remove();

            ui->listWidget->takeItem(ui->listWidget->currentRow());

            filename = filename.right(filename.count()-filename.lastIndexOf("/")-1);

            for (i=0; i<images.count(); i++) {
                checkOrgFilename = images.at(i).originalFilename.endsWith(filename);
                checkLargeURI = images.at(i).largeURI.endsWith(filename);
                if ((ui->cbOriginalFilename->isChecked() && checkOrgFilename)
                        || (!ui->cbOriginalFilename->isChecked() && checkLargeURI)) {
                    tmp = images.at(i);

                    tmp.downloaded = false;
                    tmp.requested = false;

                    images.replace(i, tmp);

                    break;
                }
            }

            download(true);
        }
    }
}

void UIImageOverview::openFile(void) {
    QString filename;
    QStringList slImageList;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        if (settings->value("options/use_internal_viewer", false).toBool()) {
            for (int i=0; i<images.length(); i++) {
                if ((images.at(i).downloaded) && !blackList->contains(images.at(i).largeURI)) {
                    slImageList << images.at(i).savedAs;
                }
            }
            imageViewer->setImageList(slImageList);
            imageViewer->setCurrentImage(ui->listWidget->currentItem()->text());
        }
        else {
            QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(filename)));
        }
    }
}

void UIImageOverview::errorHandler(QUrl url, int err) {
    switch (err) {
    case 202:
    case 404:
        if (isImage(url)) {
            blackList->add(url.toString());
            setCompleted(url.toString(), "");
        }
        else {
            // If there are still images in the list, wait until they finished (maybe they still exist)
            // else close immediately
            if (isDownloadFinished()) {
                stop();

                setTabTitle("Thread 404'ed");
                emit errorMessage("404 - Page not found");
                emit closeRequest(this, 404);
            }
            else {
                closeWhenFinished = true;
            }
        }
        break;

    case 999:
        setTabTitle("Banned");
        emit errorMessage("You are banned");
        break;

    default:
        QLOG_ERROR() << "ImageOverview :: Unhandled error (" << url.toString() << "," << err << ")";
        break;
    }
}

void UIImageOverview::messageHandler(QString m) {
    emit errorMessage(m);
}

void UIImageOverview::setTabTitle(QString s) {
    emit tabTitleChanged(this, s);
}

void UIImageOverview::setDirectory(QString d) {
    ui->leSavepath->setText(d);
}

int UIImageOverview::getDownloadedImagesCount() {
    int ret;

    ret = 0;

    for (int i=0; i<images.count(); i++) {
        if (images.at(i).downloaded)
            ret++;
    }

    return ret;
}

QString UIImageOverview::getStatus() {
    return _status;
}

QString UIImageOverview::getTitle() {
    return ui->lTitle->text();
}

QString UIImageOverview::getURI() {
    return ui->leURI->text();
}

QString UIImageOverview::getValues(void) {
    QString ret;
    QStringList list;

    list << ui->leURI->text();
    list << ui->leSavepath->text();
    list << QString("%1").arg(ui->cbRescan->isChecked());
    list << QString("%1").arg(ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt());
    list << QString("%1").arg(ui->cbOriginalFilename->isChecked());
    list << QString("%1").arg(running);
    list << QString("%1").arg(_threadBlocked);

    ret = list.join(";;");

    return ret;
}

void UIImageOverview::setValues(QString s) {
    QStringList list;
    int cbIndex;

    list = s.split(";;");

    ui->leURI->setText(list.at(0));
    ui->leSavepath->setText(list.at(1));

    cbIndex = ui->comboBox->findData(list.at(3));
    if (cbIndex != -1)
        ui->comboBox->setCurrentIndex(cbIndex);
    else
        ui->comboBox->setCurrentIndex(0);

    if (list.value(2).toInt() == 0) {
        ui->cbRescan->setChecked(false);
        ui->comboBox->setEnabled(false);
    } else {
        ui->cbRescan->setChecked(true);
        ui->comboBox->setEnabled(true);
    }

    ui->cbOriginalFilename->setChecked((bool)list.value(4).toInt());

    if (list.value(6).toInt() == 1) {
        ui->btnBlockThread->setChecked(true);
    }

    if (list.value(5) == "1") {
        start();
    }
}

void UIImageOverview::debugButton(void) {
    QLOG_ALWAYS() << Q_FUNC_INFO << ":: " << QString("%1/%2").arg(getDownloadedImagesCount()).arg(getTotalImagesCount());
}

void UIImageOverview::closeEvent(QCloseEvent *event)
{
    if (running)
        stop();

    // Delete all thumbnails
    QStringList fileList;
    for (int i=0; i<ui->listWidget->count(); i++) {
        fileList.append(tnt->getCacheFile(
                            ui->listWidget->item(i)->text()
                            )
                    );
    }
    emit removeFiles(fileList);

    event->accept();
}

void UIImageOverview::loadSettings() {
    QStringList sl;
    int index, defaultTimeout;

    sl = settings->value("options/timeout_values", (QStringList()<<"300"<<"600")).toStringList();

    ui->comboBox->clear();

    foreach (QString s, sl) {
        int i;
        bool ok;

        i = s.toInt(&ok);
        if (ok) timeoutValues<<i;
    }

    foreach (int i, timeoutValues) {
        int value;
        QString text;

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

        ui->comboBox->addItem(QString("every %1 %2").arg(value).arg(text), i);
    }

    defaultTimeout = settings->value("options/default_timeout",0).toInt();
    index = ui->comboBox->findData(defaultTimeout);
    if (index != -1) ui->comboBox->setCurrentIndex(index);

    if (defaultTimeout == 0) {
        ui->comboBox->setEnabled(false);
        ui->cbRescan->setChecked(false);
    }
    else {
        ui->comboBox->setEnabled(true);
        ui->cbRescan->setChecked(true);
    }

    setDirectory(settings->value("options/default_directory","").toString());
    useOriginalFilenames(settings->value("options/default_original_filename", false).toBool());
    updateSettings();
}

void UIImageOverview::updateSettings() {
    setThumbnailSize(QSize(settings->value("options/thumbnail_width",150).toInt(),settings->value("options/thumbnail_height",150).toInt()));
}

void UIImageOverview::useOriginalFilenames(bool b) {
    ui->cbOriginalFilename->setChecked(b);
}

void UIImageOverview::processCloseRequest() {
    stop();
    emit closeRequest(this, 0);
}

void UIImageOverview::openURI() {
    if (!ui->leURI->text().isEmpty())
        QDesktopServices::openUrl(QUrl(ui->leURI->text()));
}

void UIImageOverview::openDownloadFolder() {
    if (!ui->leSavepath->text().isEmpty())
        QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(getSavepath()), QUrl::TolerantMode));
}

void UIImageOverview::setBlackList(BlackList *bl) {
    blackList = bl;
}

void UIImageOverview::selectShortcutIndex(int idx) {
    selectShortcut(ui->cbFolderShortcuts->itemText(idx));
}

void UIImageOverview::selectShortcut(QString name) {
    QString path;

    if (path != "-----") {
        path = folderShortcuts->getPath(name);

        if (!path.isEmpty())
            ui->leSavepath->setText(path);
    }
}

void UIImageOverview::fillShortcutComboBox() {
    ui->cbFolderShortcuts->clear();
    ui->cbFolderShortcuts->addItem("-----");
    ui->cbFolderShortcuts->addItems(folderShortcuts->shortcuts());
}

void UIImageOverview::checkForFolderShortcut(QString s) {
    if (folderShortcuts->shortcutExists(s)) {
        ui->btnAddShortcut->setEnabled(false);
    }
    else {
        ui->btnAddShortcut->setEnabled(true);
    }
}

void UIImageOverview::addShortcut() {
    if (!folderShortcuts->shortcutExists(ui->leSavepath->text())) {
        folderShortcuts->addShortcut(ui->leSavepath->text(), ui->leSavepath->text());
    }
}

void UIImageOverview::startDownload(void) {
    QUrl u;

    u = QUrl(ui->leURI->text().toLatin1());
    // Plausibility of leURI was checked before at function start()

    ui->btnReloadThread->setEnabled(true);

    createSupervisedDownload(u);
}

void UIImageOverview::download(bool b) {
    if (b) {
        QString imgURI;

        downloading = true;

        while (getNextImage(&imgURI) != 0)
                createSupervisedDownload(QUrl(imgURI));
    } else {
        downloading = false;
    }
}

void UIImageOverview::stopDownload(void) {
    _IMAGE tmp;

    download(false);                // Prevent new requests
    requestHandler->cancelAll();    // Cancel pending downloads

    // Reset requested, yet not finished, downloads
    for (int i=0; i<images.length(); i++) {
        if ((images.at(i).downloaded==false) && (images.at(i).requested == true)) {
            tmp = images.at(i);
            tmp.requested = false;
            images.replace(i, tmp);
        }
    }
    follow_redirects = true;
}

void UIImageOverview::createSupervisedDownload(QUrl url) {
    if (url.isValid()) {
        if (!isImage(url)) {
            QLOG_DEBUG() << Q_FUNC_INFO << "Passing url to plugin " << url.toString();
            url = iParser->alterUrl(url);
            QLOG_DEBUG() << Q_FUNC_INFO << "URL altered to " << url.toString();
        }
        requestHandler->request(url);
    }
}

void UIImageOverview::removeSupervisedDownload(QUrl url) {
    requestHandler->cancel(url);
}

bool UIImageOverview::getUrlOfFilename(QString filename, QString * url) {
    bool ret;
    int i;

    ret = false;

    for (i=0; i<images.count(); i++) {
        if (images.at(i).savedAs == filename) {
            *url = images.at(i).largeURI;
            ret = true;
            break;
        }
    }

    return ret;
}

bool UIImageOverview::isDownloadFinished() {
    bool ret;

    ret = false;

    if (getDownloadedImagesCount() == getTotalImagesCount()) {
        ret=true;
    }

    return ret;
}

void UIImageOverview::deleteAllThumbnails() {
    QStringList fileList;

    fileList.clear();

    for (int i=0; i<ui->listWidget->count(); i++) {
        fileList.append(tnt->getCacheFile(
                            ui->listWidget->item(i)->text()
                            )
                        );
    }

    emit removeFiles(fileList);
}

void UIImageOverview::deleteAllFiles() {
    QStringList files;

    for (int i=0; i<ui->listWidget->count(); i++) {
        files.append(ui->listWidget->item(i)->text());
        files.append(tnt->getCacheFile(
                            ui->listWidget->item(i)->text()
                            )
                        );
    }

    ui->listWidget->clear();
    emit removeFiles(files);
}

void UIImageOverview::processRequestResponse(QUrl url, QByteArray ba, bool cached) {
    QString requestURI;
    QList<_IMAGE>   imageList;
    QList<QUrl>     threadList;
    ParsingStatus   status;
    QString         path;
    qint64 bytesWritten;

    requestURI = url.toString();
    path = url.path();
    QLOG_TRACE() << "UIImageOverview :: Got response for " << url.toString() << ":" << QString(ba);
    if (!_threadBlocked) {
        if (isImage(url)) {
            QFile f;
            QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
            QStringList res;
            int pos;

            pos = 0;

            pos = rx.indexIn(requestURI);
            res = rx.capturedTexts();

            if (pos != -1) {
                f.setFileName(getSavepath()+"/"+res.at(1)+res.at(2));

                if (ui->cbOriginalFilename->isChecked()) {
                    _IMAGE tmp;

                    for (int i=0; i<images.count(); i++) {
                        if (images.at(i).largeURI.endsWith("/"+res.at(1)+res.at(2))) {
                            tmp = images.at(i);

                            f.setFileName(getSavepath()+"/"+tmp.originalFilename);
                            break;
                        }
                    }
                }

                f.open(QIODevice::ReadWrite);
                bytesWritten = f.write(ba);
                f.close();

                if (bytesWritten == ba.size()) {
                    createThumbnail(f.fileName());
                    setCompleted(requestURI, f.fileName());
                }
                else {
                    QLOG_ERROR() << "UIImageOverview :: Couldn't save file from URI " << url.toString();
                }
            }

        }
        else {
            setStatus("Parsing");
            iParser->setURL(url);
            status = iParser->parseHTML(ba);

            if (status.hasErrors) {
                QLOG_ERROR() << "ImageOverview :: Parser error " << iParser->getErrorCode();
                switch (iParser->getErrorCode()) {
                case 404:
                    stopDownload();
                    processCloseRequest();
                    break;

                default:
                    break;
                }
            }
            else {
                _cachedResult = cached;

                if (status.isFrontpage) {
                    QStringList newTab;
                    QString v;

                    v = getValues();
                    newTab = v.split(";;");
                    threadList = iParser->getUrlList();

                    foreach (QUrl u, threadList) {
                        newTab.replace(0, u.toString());
                        emit createTabRequest(newTab.join(";;"));
                    }

                    if (settings->value("options/close_overview_threads", true).toBool()) {
                        emit closeRequest(this, 0);
                    }
                    else {
                        if (status.hasTitle) {
                            ui->lTitle->setText(HTML::decode(iParser->getThreadTitle()));
                            ui->lTitle2->setText(HTML::decode(iParser->getThreadTitle()));
                        }
                    }
                }
                else if (status.threadFragmented && follow_redirects) {
                    follow_redirects = false;
                    threadList = iParser->getUrlList();
                    QLOG_INFO() << Q_FUNC_INFO << ":: redirect list " << threadList;
                    foreach (QUrl u, threadList) {
                        createSupervisedDownload(u);
                    }
                }
                else if (status.hasRedirect && follow_redirects) {
                    ui->leURI->setText(iParser->getRedirectURL().toString());
                    QLOG_INFO() << Q_FUNC_INFO << ":: redirecting to " << iParser->getRedirectURL().toString();
                    stop();
                    start();
                }
                else {
                    if (status.hasImages) {
                        imageList = iParser->getImageList();
                        mergeImageList(imageList);
                    }
                    else {
                        setStatus("idle");
                    }

                    if (status.hasTitle) {
                        ui->lTitle->setText(HTML::decode(iParser->getThreadTitle()));
                        ui->lTitle2->setText(HTML::decode(iParser->getThreadTitle()));
                    }
                }
            }

            if (cached) {
                timer->stop();
                ui->cbRescan->setChecked(false);
                setStatus("Cached");
                ui->btnReloadThread->setEnabled(false);
            }
        }
    }
}

void UIImageOverview::setCompleted(QString uri, QString filename) {
    int i;
    _IMAGE tmp;

    for (i=0; i<images.length(); i++) {
        if (images.at(i).largeURI == uri) {
            tmp = images.at(i);
            tmp.downloaded = true;
            tmp.savedAs = filename;

            images.replace(i,tmp);

            updateDownloadStatus();

            if (isDownloadFinished()) {
                download(false);
            }

            break;
        }
    }
}

bool UIImageOverview::isImage(QUrl url) {
    bool ret;

    ret = false;

    if (url.toString().indexOf(QRegExp(__IMAGE_REGEXP__, Qt::CaseInsensitive)) != -1)
        ret = true;

    return ret;
}

bool UIImageOverview::getNextImage(QString* s) {
    int i;
    bool ret;
    _IMAGE tmp;
    QFile f;
    QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
    QStringList res;
    int pos;

    ret = false;

    if (downloading) {
        for (i=0; i<images.length(); i++) {
            if (!images.at(i).downloaded && !images.at(i).requested) {
                tmp = images.at(i);
                tmp.requested = true;
                images.replace(i,tmp);

                // Check if file already exists in destination dir
                pos = 0;

                pos = rx.indexIn(tmp.largeURI);
                res = rx.capturedTexts();

                if (pos != -1) {
                    f.setFileName(getSavepath()+res.at(0));
                }

                if (f.exists()) {
                    tmp.downloaded = true;
                    images.replace(i, tmp);

                    createThumbnail(f.fileName());
                    setCompleted(tmp.largeURI, f.fileName());
                }
                else {
                    *s = tmp.largeURI;
                    ret = true;
                    break;
                }
            }
        }
    }

    return ret;
}

bool UIImageOverview::selectParser(QUrl url) {
    bool ret;
    ParserPluginInterface* tmp;

    if (url.isEmpty())
        url = QUrl(ui->leURI->text().toLatin1());

    tmp = pluginManager->getParser(url, &ret);
    if (ret) {
        oParser = tmp->createInstance();
        iParser = qobject_cast<ParserPluginInterface*>(oParser);
        iParser->setURL(url);
    }

    return ret;
}

void UIImageOverview::mergeImageList(QList<_IMAGE> list) {
    bool imagesAdded;

    imagesAdded = false;

    for (int i=0; i<list.count(); i++) {
        if(addImage(list.at(i)))
            imagesAdded = true;
    }

    if (imagesAdded) {
        download(true);
    }
    else {
        if (isDownloadFinished()) {
            download(false);
        }
    }

    updateDownloadStatus();
}

bool UIImageOverview::addImage(_IMAGE img) {
    int i,k;
    bool alreadyInList;
    bool fileExists;

    fileExists = false;

    if (blackList->contains(img.largeURI)) {
        alreadyInList = true;
    }
    else {
        alreadyInList = false;
        k = 2;
        for (i=0; i<images.length(); i++) {
            if (images.at(i).largeURI == img.largeURI) {
                alreadyInList = true;
                break;
            }

            if (images.at(i).originalFilename == img.originalFilename) {
                QStringList tmp;

                tmp = img.originalFilename.split(QRegExp("\\(\\d+\\)"));
                if  (tmp.count() > 1) // Already has a number in brackets in filename
                    img.originalFilename = QString("%1(%2)%3").arg(tmp.at(0)).
                                           arg(k++).
                                           arg(tmp.at(1));
                else
                    img.originalFilename = img.originalFilename.replace("."," (1).");
            }
        }

        if (!alreadyInList) {
            // Check if already downloaded
            QFile f;
            if (ui->cbOriginalFilename->isChecked()) {
                f.setFileName(getSavepath()+"/"+img.originalFilename);
            }
            else {
                QRegExp rx(__IMAGEFILE_REGEXP__, Qt::CaseInsensitive, QRegExp::RegExp2);
                QStringList res;
                int pos;

                pos = 0;

                pos = rx.indexIn(img.largeURI);
                res = rx.capturedTexts();

                if (pos != -1) {
                    f.setFileName(getSavepath()+"/"+res.at(1)+res.at(2));
                }
            }

            if (f.exists()) {
                img.downloaded = true;
                fileExists = true;
                img.savedAs = f.fileName();
            }
            else if (_cachedResult) {
                blackList->add(img.largeURI);
                img.savedAs = "";
                img.downloaded = true;
                fileExists = true;

                QLOG_INFO() << Q_FUNC_INFO << ":: Image " << img.largeURI << " from cached result does not exists. Looked in " << getSavepath();
            }

            images.append(img);

            if (fileExists && img.savedAs != "") {
                createThumbnail(f.fileName());
            }
        }
    }

    return (!alreadyInList && !fileExists);
}

bool UIImageOverview::checkForExistingThread(QString s) {
    bool ret;

    ret = false;
/*
    ret = mainWindow->threadExists(ui->leURI->text());

    if (ret) {
        ui->lTitle->setText("<span style=\"font-weight:600; color:#ff0000;\">Thread does already exists!</span>");
        ui->lTitle->setTextFormat(Qt::AutoText);
        ui->btnStart->setEnabled(false);
        setStatus("Thread already exists");
        emit errorMessage("Thread already exists");
        emit tabTitleChanged(this, "Thread already exists");
    }
    else {
        ui->btnStart->setEnabled(true);
        ui->lTitle->setText("");
    }
*/
    return ret;
}

void UIImageOverview::updateDownloadStatus() {
    int d, t;

    d = getDownloadedImagesCount();
    t = getTotalImagesCount();
//    updateExpectedThumbnailCount();

    if (!isDownloadFinished()) {
        setTabTitle(QString("%1/%2").arg(d).arg(t));
        setStatus("downloading");
        ui->progressBar->setVisible(true);
        ui->progressBar->setValue(d);
        ui->progressBar->setMaximum(t);
    }
    else {
        ui->progressBar->setVisible(false);

        QLOG_DEBUG() << "UIImageOverview :: updateDownloadStatus() :: item count: " << ui->listWidget->count() << "; expectedThumbnailCount " << expectedThumbnailCount;
        if (missingThumbs.count() == 0) {
            if (_cachedResult) {
                setTabTitle("Cached");
                setStatus("Cached");
            }
            else {
                setTabTitle("Finished");
                setStatus("Finished");
            }
        }
        else {
            setTabTitle("Rendering");
            setStatus("Rendering");
        }
    }

    emit changed();
}

void UIImageOverview::setStatus(QString s) {
     _status = s;
     emit changed();
}

void UIImageOverview::rebuildThumbnails() {
    deleteAllThumbnails();
    ui->listWidget->clear();
    expectedThumbnailCount = 0;
    thumbnailCount = 0;

    for (int i=0; i<images.length(); i++) {
        if (images.at(i).downloaded && !images.at(i).savedAs.isEmpty()) {
            createThumbnail(images.at(i).savedAs);
        }
    }
}

void UIImageOverview::reloadThread() {
    _IMAGE t;
    deleteAllThumbnails();
    ui->listWidget->clear();

    for (int i=0; i<images.length(); i++) {
        if (images.at(i).downloaded) {
            t = images.at(i);

            t.downloaded = false;
            t.requested = false;

            images.replace(i,t);
            QFile::remove(t.savedAs);
        }
    }

    download(true);
}

QString UIImageOverview::getSavepath() {
    QString ret;

    if (iParser) {
        ret = iParser->parseSavepath(ui->leSavepath->text());
    }
    else {
        QLOG_WARN() << "UIImageOverview :: Called getSavepath() although no parser was loaded.";
        ret = ui->leSavepath->text();
    }

    return ret;
}

void UIImageOverview::updateExpectedThumbnailCount() {
    expectedThumbnailCount=0;
    for (int i=0; i<images.length(); i++) {
        if (images.at(i).downloaded &&
                images.at(i).savedAs != "") {
            expectedThumbnailCount++;
        }
    }
}

void UIImageOverview::showImagePreview() {
    QStringList slImageList;

    if (settings->value("options/use_internal_viewer", false).toBool()) {
        for (int i=0; i<images.length(); i++) {
            if ((images.at(i).downloaded) && !blackList->contains(images.at(i).largeURI)) {
                slImageList << images.at(i).savedAs;
            }
        }

        imageViewer->setImageList(slImageList);

        if (slImageList.count() > 0) {
            imageViewer->setCurrentImage(0);
        }
    }
}

void UIImageOverview::blockThread(bool b) {
    _threadBlocked = b;
    if (_threadBlocked) {
        QLOG_INFO() << "UIImageOverview :: Blocking thread";
        running = false;
        stopDownload();
        setStatus("Blocked");
    }
    else {
        QLOG_INFO() << "UIImageOverview :: Unblocking thread";

        // Make it a fresh start for this images thread
        images.clear();
        deleteAllThumbnails();
        ui->listWidget->clear();
        start();
    }
}

bool UIImageOverview::hasNewImages() {
    bool ret;

    ret = false;

    QLOG_DEBUG() << Q_FUNC_INFO << "tnCountLast: " << thumbnailCountLastViewed << "; tnCount: " << thumbnailCount;
    if (thumbnailCountLastViewed < thumbnailCount) {
        ret = true;
    }

    return ret;
}

void UIImageOverview::threadViewed() {
    thumbnailCountLastViewed = thumbnailCount;
    emit changed();
}
