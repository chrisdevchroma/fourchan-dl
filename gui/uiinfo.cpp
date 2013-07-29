#include "uiinfo.h"
#include "ui_uiinfo.h"
UIInfo::UIInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIInfo)
{
    QString text;
    ui->setupUi(this);
    updateStatistics();
    uiPendingRequests = new UIPendingRequests(this);

    timer = new QTimer(this);
    timer->setInterval(3000);
    timer->setSingleShot(false);
    timer->start();

    text = ui->label_2->text();
#ifdef __DEBUG__
    text.replace("%%VERSION%%", "DEBUG");
#else
    text.replace("%%VERSION%%", PROGRAM_VERSION);
#endif
    text.replace("%%BUILDDATE%%", BUILDDATE);
    text.replace("%%QT_VERSION%%", QT_VERSION_STR);
    ui->label_2->setText(text);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatistics()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDebugInformation()));

    connect(ui->btnShowRequests, SIGNAL(clicked()), this, SLOT(showRequests()));
    connect(uiPendingRequests, SIGNAL(reloadRequested()), this, SLOT(reloadRequests()));
    connect(this, SIGNAL(rejected()), uiPendingRequests, SLOT(hide()));

    logFile = new QFile();
    logFile->setFileName("fourchan-dl.log");
    logFile->open(QIODevice::ReadOnly | QIODevice::Unbuffered | QIODevice::Text);

    if (logFile->isReadable()) {
        connect(timer, SIGNAL(timeout()), this, SLOT(updateLogFile()));
    }
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
    QMap<QString,QString> replacementStrings;

    twi = 0;
    keys = components.keys();
    QTreeWidgetItem* pluginTree = new QTreeWidgetItem(ui->pluginInfo);
    pluginTree->setText(0, "Plugins");
    QTreeWidgetItem* executableTree = new QTreeWidgetItem(ui->pluginInfo);
    executableTree->setText(0, "Executables");
    QTreeWidgetItem* qtTree = new QTreeWidgetItem(ui->pluginInfo);
    qtTree->setText(0, "Qt");
    QTreeWidgetItem* libTree = new QTreeWidgetItem(ui->pluginInfo);
    libTree->setText(0, "External libraries");
    QTreeWidgetItem* unknownTree = new QTreeWidgetItem(ui->pluginInfo);
    unknownTree->setText(0, "Not categorized");

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

                QTreeWidgetItem* replaceChars = new QTreeWidgetItem(pluginName);
                replaceChars->setText(0,"Available formatting");
                replacementStrings.clear();
                replacementStrings = p->getSupportedReplaceCharacters();

                QMapIterator<QString,QString> i(replacementStrings);
                while (i.hasNext()) {
                    i.next();
                    QTreeWidgetItem* rc_twi = new QTreeWidgetItem(replaceChars);
                    rc_twi->setText(0, i.key());
                    rc_twi->setText(1, i.value());
                }
            }
        }
        else {
            if (c.type.startsWith("executable")) {
                twi = executableTree;
            } else if (c.type.startsWith("qt")) {
                twi = qtTree;
            } else if (c.type.startsWith("library")) {
                twi = libTree;
            } else {
                twi = unknownTree;
            }

            QTreeWidgetItem* componentName = new QTreeWidgetItem(twi);
            componentName->setText(0, c.componentName);

            if (!c.version.isEmpty()) {
                QTreeWidgetItem* version = new QTreeWidgetItem(componentName);
                version->setText(0, "Version");
                version->setText(1, c.version);
            }
        }
//        ui->pluginInfo->addTopLevelItem(pluginTree);
//        ui->pluginInfo->addTopLevelItem(qtTree);
//        ui->pluginInfo->addTopLevelItem(executableTree);
    }

//    ui->pluginInfo->expandAll();
}

void UIInfo::updateLogFile() {
    if (logFile->isReadable()) {
        while (!logFile->atEnd()) {
            ui->textEdit->append(QString(logFile->readLine()).simplified());
        }
//        ui->textEdit->setText(QString(logFile->readAll()));
    }
}

UIInfo::~UIInfo()
{
    logFile->close();
    delete ui;
}

void UIInfo::showRequests() {
    reloadRequests();
    uiPendingRequests->show();
}

void UIInfo::reloadRequests() {
    uiPendingRequests->showPendingRequestList(downloadManager->getPendingRequestsMap());
}
