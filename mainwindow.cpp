#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    uiConfig = new UIConfig(this);

    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    qDebug()<< "Settings status" << settings->status();

    ui->tabWidget->removeTab(0);

    uiConfig->setSettings(settings);

    loadOptions();
    restoreSettings();

    updateThumbnailSize();
    updateMaxDownloads();

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(uiConfig, SIGNAL(configurationChanged()), this, SLOT(loadOptions()));
}

int MainWindow::addTab() {
    int ci;
    UI4chan* w;

    w = new UI4chan(this);

    ci = ui->tabWidget->addTab(w, "no name");
    w->setDirectory(defaultDirectory);
    w->setSettings(settings);
    w->setThumbnailSize(thumbnailSize);
    w->setMaxDownloads(maxDownloads);

    connect(w, SIGNAL(errorMessage(QString)), this, SLOT(displayError(QString)));
    connect(w, SIGNAL(tabTitleChanged(UI4chan*, QString)), this, SLOT(changeTabTitle(UI4chan*, QString)));
    connect(w, SIGNAL(closeRequest(UI4chan*)), this, SLOT(processCloseRequest(UI4chan*)));
    connect(w, SIGNAL(directoryChanged(QString)), this, SLOT(setDefaultDirectory(QString)));

    ui->tabWidget->setCurrentIndex(ci);

    changeTabTitle(w, "idling");

    return ci;
}

void MainWindow::closeTab(int i) {
    UI4chan* w;

    if (ui->tabWidget->count()>1) {
        ui->tabWidget->setCurrentIndex(i);
        w = (UI4chan*)ui->tabWidget->widget(i);
        w->close();
        ui->tabWidget->removeTab(i);
    }
}

void MainWindow::displayError(QString s) {
    ui->statusBar->showMessage(s, 3000);
}

void MainWindow::showInfo(void) {
    UIInfo* uiInfo;

    uiInfo = new UIInfo(this);
    uiInfo->show();
}

void MainWindow::showConfiguration(void) {
    uiConfig->show();
}

void MainWindow::setDefaultDirectory(QString d) {
    defaultDirectory = d;
}

void MainWindow::changeTabTitle(UI4chan* w, QString s) {
    int i;

    i = ui->tabWidget->indexOf((QWidget*)w);
    ui->tabWidget->setTabText(i, s);
}

void MainWindow::restoreSettings(void) {
    // Restore window position
    QPoint p;
    QSize s;
    int state;
    int tabCount;

    settings->beginGroup("window");
        p = settings->value("position",QPoint(0,0)).toPoint();
        state = settings->value("state",0).toInt();
        s = settings->value("size",QSize(0,0)).toSize();
    settings->endGroup();

    if (p != QPoint(0,0))
        this->move(p);

    if (s != QSize(0,0) && state != Qt::WindowNoState)
        this->resize(s);

    if (state != Qt::WindowNoState)
        this->setWindowState((Qt::WindowState) state);


    // tabs
    tabCount = settings->value("tabs/count",0).toInt();
    if (settings->value("options/resume_session", false).toBool() && tabCount > 0) {
        int ci;

        for (int i=0; i<tabCount; i++) {
            ci = addTab();

            ((UI4chan*)ui->tabWidget->widget(ci))->setValues(
                    settings->value(QString("tabs/tab%1").arg(i), ";;;;0;;every 30 seconds;;0").toString()
                    );
        }
    } else {
        addTab();
    }
}

void MainWindow::saveSettings(void) {
    // Window related stuff
    settings->beginGroup("window");
        settings->setValue("position", this->pos());
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

        updateThumbnailSize();
        updateMaxDownloads();
    settings->endGroup();
}

void MainWindow::processCloseRequest(UI4chan* w) {
    if (settings->value("options/automatic_close", false).toBool()) {
        int i;

        i = ui->tabWidget->indexOf((QWidget*)w);
        w->close();
        ui->tabWidget->removeTab(i);
    }
}

void MainWindow::updateThumbnailSize(void) {
    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UI4chan*)ui->tabWidget->widget(i))->setThumbnailSize(thumbnailSize);
    }
}

void MainWindow::updateMaxDownloads(void) {
    for (int i=0; i<ui->tabWidget->count(); i++) {
        ((UI4chan*)ui->tabWidget->widget(i))->setMaxDownloads(maxDownloads);
    }
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete ui;
}
