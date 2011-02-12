#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->removeTab(0);
    addTab();

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

int MainWindow::addTab() {
    int ci;

    ci = ui->tabWidget->addTab(new UI4chan(), "no name");

    connect(ui->tabWidget->widget(ci), SIGNAL(errorMessage(QString)), this, SLOT(displayError(QString)));
    connect(ui->tabWidget->widget(ci), SIGNAL(tabTitleChanged(QString)), this, SLOT(changeTabTitle(QString)));

    ui->tabWidget->setCurrentIndex(ci);

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

void MainWindow::changeTabTitle(QString s) {
    qDebug() << s;
}

MainWindow::~MainWindow()
{
    delete ui;
}
