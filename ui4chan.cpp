#include "ui4chan.h"
#include "ui_ui4chan.h"

UI4chan::UI4chan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UI4chan)
{
    QClipboard *clipboard = QApplication::clipboard();

    ui->setupUi(this);
    p = new Parser();
    timer = new QTimer();

    iconSize.setHeight(200);
    iconSize.setWidth(200);
    thumbnailsizeLocked = false;

    deleteFileAction = new QAction(QString("Delete File"), this);
    deleteFileAction->setIcon(QIcon(":/icons/resources/remove.png"));
    reloadFileAction = new QAction("Reload File", this);
    reloadFileAction->setIcon(QIcon(":/icons/resources/reload.png"));
    openFileAction = new QAction("Open File", this);
    openFileAction->setIcon(QIcon(":/icons/resources/open.png"));

    timeoutValues << 30 << 60 << 120 << 300 << 600;

    foreach (int i, timeoutValues) {
        if (i <= 60)
            ui->comboBox->addItem(QString("every %1 seconds").arg(i));
        else
            ui->comboBox->addItem(QString("every %1 minutes").arg(i/60), i);
    }

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(-1);
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);

    if (clipboard->text().contains("http://boards.4chan.org"))
        ui->leURI->setText(clipboard->text());

    connect(p, SIGNAL(downloadedCountChanged(int)), ui->progressBar, SLOT(setValue(int)));
    connect(p, SIGNAL(totalCountChanged(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(p, SIGNAL(finished()), this, SLOT(downloadsFinished()));
    connect(p, SIGNAL(fileFinished(QString)), this, SLOT(addThumbnail(QString)));
    connect(p, SIGNAL(error(int)), this, SLOT(errorHandler(int)));
    connect(p, SIGNAL(threadTitleChanged(QString)), ui->lTitle, SLOT(setText(QString)));
    connect(p, SIGNAL(tabTitleChanged(QString)), this, SLOT(setTabTitle(QString)));
    connect(ui->cbOriginalFilename, SIGNAL(stateChanged(int)), p, SLOT(setUseOriginalFilename(int)));

    connect(deleteFileAction, SIGNAL(triggered()), this, SLOT(deleteFile()));
    connect(reloadFileAction, SIGNAL(triggered()), this, SLOT(reloadFile()));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));

    connect(timer, SIGNAL(timeout()), this, SLOT(triggerRescan()));

    setTabTitle("idling");
}

UI4chan::~UI4chan()
{
    delete ui;
}

bool UI4chan::setThumbnailSize(QSize s) {
    bool ret;

    ret = false;

    if (!thumbnailsizeLocked) {
        iconSize = s;
        ui->listWidget->setIconSize(iconSize);
        ui->listWidget->setGridSize(QSize(iconSize.width()+10,iconSize.height()+10));
        ret = true;
    }

    return ret;
}

void UI4chan::start(void) {
    QDir dir;

    if (ui->leURI->text() != "") {
        ui->leURI->setEnabled(false);
        p->setURI(ui->leURI->text());
        dir.setPath(ui->leSavepath->text());

        if (!dir.exists()) {
            QDir d;

            d.mkpath(ui->leSavepath->text());
        }

        if (dir.exists()) {
            ui->leSavepath->setEnabled(false);
            p->setSavePath(ui->leSavepath->text());
            p->start();

            ui->btnStart->setEnabled(false);
            ui->btnStop->setEnabled(true);
            ui->cbRescan->setEnabled(false);
            ui->comboBox->setEnabled(false);
            ui->progressBar->setEnabled(true);
            ui->cbOriginalFilename->setEnabled(false);

            if (ui->cbRescan->isChecked()) {
                timer->setInterval(timeoutValues.at(ui->comboBox->currentIndex())*1000);

                timer->start();
            }
        }
        else
        {
            emit errorMessage("Directory does not exist / Could not be created");
        }
    }
}

void UI4chan::stop(void) {
    p->stop();
    timer->stop();
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->leURI->setEnabled(true);
    ui->leSavepath->setEnabled(true);
    ui->cbRescan->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->progressBar->setEnabled(false);
    ui->cbOriginalFilename->setEnabled(true);
}

void UI4chan::chooseLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose storage directory", ui->leSavepath->text());

    ui->leSavepath->setText(loc);
}

void UI4chan::triggerRescan(void) {
    p->start();
    timer->start();

    setTabTitle("rescanning");
}

void UI4chan::addThumbnail(QString filename) {
    QListWidgetItem* item;
    QPixmap pixmapLarge, pixmapSmall;

    pixmapLarge.load(filename);

    if (pixmapLarge.width()<iconSize.width()
        && pixmapLarge.height()<iconSize.height()
        && !(settings->value("options/enlarge_thumbnails", false).toBool())) {
        pixmapSmall = pixmapLarge;
    } else {
        if (settings->value("options/hq_thumbnails", false).toBool())
            pixmapSmall = pixmapLarge.scaled(iconSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        else
            pixmapSmall = pixmapLarge.scaled(iconSize,Qt::KeepAspectRatio,Qt::FastTransformation);
    }
    item = new QListWidgetItem(
            QIcon(pixmapSmall),
            filename,
            ui->listWidget);

    ui->listWidget->addItem(item);
    thumbnailsizeLocked = true;
}

void UI4chan::downloadsFinished() {
    setTabTitle("idling");

    if (!ui->cbRescan->isChecked())
        stop();
}

void UI4chan::on_listWidget_customContextMenuRequested(QPoint pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    contextMenu.addAction(openFileAction);
    contextMenu.addAction(deleteFileAction);
    contextMenu.addAction(reloadFileAction);
    contextMenu.exec(ui->listWidget->mapTo(this,mapToGlobal(pos)));
}

void UI4chan::deleteFile(void) {
    QFile f;
    QString filename;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        f.setFileName(filename);

        if (f.exists()) {
            f.remove();

            ui->listWidget->takeItem(ui->listWidget->currentRow());
        }
    }
}

void UI4chan::reloadFile(void) {
    QString filename;
    QFile f;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        f.setFileName(filename);

        if (f.exists()) {
            f.remove();

            ui->listWidget->takeItem(ui->listWidget->currentRow());

            p->reloadFile(filename);
        }
    }
}

void UI4chan::openFile(void) {
    QString filename;

    filename = ui->listWidget->currentItem()->text();
    if (filename != "") {
        QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(filename)));
    }
}

void UI4chan::errorHandler(int err) {
    switch (err) {
    case 404:
        p->stop();
        stop();

        setTabTitle("Thread 404'ed");
        emit errorMessage("404 - Page not found");
        emit closeRequest(this);

        break;

    default:
        break;
    }
}

void UI4chan::setTabTitle(QString s) {
    emit tabTitleChanged(this, s);
}

void UI4chan::labelDirectoryChanged(QString s) {
    emit directoryChanged(s);
}

void UI4chan::setDirectory(QString d) {
    ui->leSavepath->setText(d);
}

QString UI4chan::getValues(void) {
    QString ret;
    QStringList list;

    list << ui->leURI->text();
    list << ui->leSavepath->text();
    list << QString("%1").arg(ui->cbRescan->checkState());
    list << ui->comboBox->currentText();
    list << QString("%1").arg(ui->cbOriginalFilename->checkState());

    ret = list.join(";;");

    return ret;
}

void UI4chan::setValues(QString s) {
    QStringList list;

    list = s.split(";;");

    ui->leURI->setText(list.at(0));
    ui->leSavepath->setText(list.at(1));
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(list.at(3)));

    if (list.value(2).toInt() == 0) {
        ui->cbRescan->setChecked(false);
        ui->comboBox->setEnabled(false);
    } else {
        ui->cbRescan->setChecked(true);
        ui->comboBox->setEnabled(true);
    }

    ui->cbOriginalFilename->setCheckState((Qt::CheckState)(list.value(4).toInt()));
}

void UI4chan::setMaxDownloads(int i) {
    p->setMaxDownloads(i);
}

void UI4chan::setSettings(QSettings* s) {
    settings = s;
}
