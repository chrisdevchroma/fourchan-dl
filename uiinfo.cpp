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
    loadPluginInfo();
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

void UIInfo::loadPluginInfo() {
    int pluginCount;
    ParserPluginInterface* p;

    pluginCount = pluginManager->getAvailablePlugins().count();
    for (int i=0; i<pluginCount; i++) {
        p = pluginManager->getPlugin(i);

        if (p != 0) {
            QTreeWidgetItem* pluginName = new QTreeWidgetItem(ui->pluginInfo);
            pluginName->setText(0, p->getPluginName());

            QTreeWidgetItem* author = new QTreeWidgetItem(pluginName);
            author->setText(0, "Author");
            author->setText(1, p->getAuthor());

            QTreeWidgetItem* version = new QTreeWidgetItem(pluginName);
            version->setText(0, "Version");
            version->setText(1, p->getVersion());

            QTreeWidgetItem* iface = new QTreeWidgetItem(pluginName);
            iface->setText(0, "Interface Revision");
            iface->setText(1, p->getInterfaceRevision());

            QTreeWidgetItem* domain = new QTreeWidgetItem(pluginName);
            domain->setText(0, "Domain");
            domain->setText(1, p->getDomain());

            ui->pluginInfo->addTopLevelItem(pluginName);
        }
    }
}

UIInfo::~UIInfo()
{
    delete ui;
}
