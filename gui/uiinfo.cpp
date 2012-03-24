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
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDebugInformation()));
}

void UIInfo::setCurrentVersion(QString s) {
    QString text;

    text = ui->label_2->text();
    text.replace("%%CURRENT_VERSION%%", s);
    ui->label_2->setText(text);
}

void UIInfo::updateStatistics() {
    QString s;
    QStringList sizePostfix;
    int files, idx;
    float kbyte;

    sizePostfix << "KB" << "MB" << "GB" << "TB";

    files = 0;
    kbyte = 0;

    downloadManager->getStatistics(&files, &kbyte);

    for (idx=0; idx<sizePostfix.size(); idx++) {
        if (kbyte>1024.0) {
            kbyte /= 1024.0;
        }
        else {
            break;
        }
    }

    s = QString("You have downloaded\n%1 files\n%2 %3").arg(files).arg(kbyte).arg(sizePostfix.value(idx));

    ui->lStatistics->setText(s);
}

void UIInfo::updateDebugInformation() {
    QString str;

    str = QString("Pending requests: %1<br />" \
                  "Running requests: %2")
            .arg(downloadManager->getPendingRequests())
            .arg(downloadManager->getRunningRequests());

    ui->lDebugInformation->setText(str);
}

void UIInfo::loadComponentInfo(QMap<QString, component_information> components) {
    component_information c;
    ParserPluginInterface* p;
    QTreeWidgetItem* twi;
    QStringList keys;

    twi = 0;
    keys = components.keys();
    QTreeWidgetItem* pluginTree = new QTreeWidgetItem(ui->pluginInfo);
    pluginTree->setText(0, "Plugins");
    QTreeWidgetItem* executableTree = new QTreeWidgetItem(ui->pluginInfo);
    executableTree->setText(0, "Executables");
    QTreeWidgetItem* qtTree = new QTreeWidgetItem(ui->pluginInfo);
    qtTree->setText(0, "Qt");

    foreach (QString key, keys) {
        c = components.value(key);

        if (c.type.startsWith("plugin")) {
            p = pluginManager->getPlugin(key);

            if (p != 0) {
                QTreeWidgetItem* pluginName = new QTreeWidgetItem(pluginTree);
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
            }
        }
        else {
            if (c.type.startsWith("executable")) twi = executableTree;
            else if (c.type.startsWith("qt")) twi = qtTree;

            QTreeWidgetItem* componentName = new QTreeWidgetItem(twi);
            componentName->setText(0, c.componentName);

            QTreeWidgetItem* version = new QTreeWidgetItem(componentName);
            version->setText(0, "Version");
            version->setText(1, c.version);
        }
        ui->pluginInfo->addTopLevelItem(pluginTree);
        ui->pluginInfo->addTopLevelItem(qtTree);
        ui->pluginInfo->addTopLevelItem(executableTree);
    }

//    ui->pluginInfo->expandAll();
}

UIInfo::~UIInfo()
{
    delete ui;
}
