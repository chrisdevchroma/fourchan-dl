#include "uithreadadder.h"
#include "ui_uithreadadder.h"

UIThreadAdder::UIThreadAdder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIThreadAdder)
{
    ui->setupUi(this);
    clipboard = QApplication::clipboard();
    settings = new QSettings("settings.ini", QSettings::IniFormat);

//    connect(clipboard, SIGNAL(changed(QClipboard::Mode))), this, SLOT(checkClipboard(QClipboard::Mode));
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(checkClipboard()));
    connect(folderShortcuts, SIGNAL(shortcutsChanged()), this, SLOT(fillShortcutComboBox()));
    connect(ui->cbFolderShortcuts, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectShortcut(QString)));

    loadSettings();
    fillShortcutComboBox();
}

UIThreadAdder::~UIThreadAdder()
{
    delete ui;
}

void UIThreadAdder::checkClipboard() {
    QStringList links;
    QString text;
    QList<QUrl> urls;
    QClipboard::Mode mode;

    mode = QClipboard::Clipboard;
    if (clipboard->mimeData(mode)->hasText() || clipboard->mimeData(mode)->hasHtml()) {
        text = clipboard->mimeData(mode)->text();
        links = parseText(text);
    }
    else if (clipboard->mimeData(mode)->hasUrls()) {
        urls = clipboard->mimeData(mode)->urls();
    }

    foreach(QString link, links) {
        QListWidgetItem* item;

        item = new QListWidgetItem(ui->listWidget);
        item->setText(link);
        item->setCheckState(Qt::Checked);
        ui->listWidget->addItem(item);
    }
}

QStringList UIThreadAdder::parseURLList(QList<QUrl> urls) {
    QStringList ret;

    foreach (QUrl url, urls) {
        ret << url.toString();
    }

    return ret;
}

QStringList UIThreadAdder::parseText(QString text) {
    QStringList ret;
    QStringList res;
    QString url;
    QRegExp rx("(((http://www)|(http://)|(www))[-a-zA-Z0-9%_\\+.~?&//=]+)", Qt::CaseInsensitive, QRegExp::RegExp2);
    int pos;

    // Insert word boundaries for RegExp
    text = text.prepend(" ");
    text = text.append(" ");

    pos = 0;
    while (pos > -1) {
        pos = rx.indexIn(text, pos+1);
        res = rx.capturedTexts();

        url = res.at(0);

        if (url.endsWith("/"))
            url.remove(-1,1);

        if (url.contains(QRegExp("(static|thumbs)", Qt::CaseInsensitive, QRegExp::RegExp2)))
            url.clear();
/*
        if ((url.startsWith("http") || url.startsWith("www")) && !url.contains("4chan.org"))
            url.clear();
*/
        if (!url.isEmpty())
            ret << url.trimmed();
    }

    ret.removeDuplicates();

    return ret;
}

void UIThreadAdder::selectAll() {
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void UIThreadAdder::selectNone() {
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void UIThreadAdder::startAll() {
    selectAll();
    startSelected();
}

void UIThreadAdder::startSelected() {
    QStringList threadSettings;

    threadSettings << "";
    threadSettings << ui->leSavepath->text();
    threadSettings << QString("%1").arg(ui->cbRescan->checkState());
    threadSettings << QString("%1").arg(ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt());
    threadSettings << QString("%1").arg(ui->cbOriginalFilename->checkState() == Qt::Checked);
    threadSettings << QString("%1").arg(ui->cbStartImmediately->checkState() == Qt::Checked);

    if (readyToStart()) {
        for (int i=0; i<ui->listWidget->count(); i++) {
            if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
                threadSettings.replace(0, ui->listWidget->item(i)->text());

                emit addTab(threadSettings.join(";;"));
            }
        }
    }

    clearSelected();
    this->hide();
}

void UIThreadAdder::clearSelected() {
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) {
            ui->listWidget->takeItem(i--);
        }
    }

}

bool UIThreadAdder::readyToStart() {
    bool ret;

    ret = false;

    if (!ui->leSavepath->text().isEmpty()) {
        ret = true;
    }
    else {
        ret = false;
    }

    ui->btnStartAll->setEnabled(ret);
    ui->btnStartSelected->setEnabled(ret);

    return ret;
}

void UIThreadAdder::chooseLocation(void) {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose storage directory", ui->leSavepath->text());

    if (loc != "") {
        if (loc.endsWith("\\"))
            loc.chop(1);
        ui->leSavepath->setText(loc);

        readyToStart();

        emit directoryChanged(loc);
    }
}

void UIThreadAdder::loadSettings() {
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

    ui->leSavepath->setText(settings->value("options/default_directory","").toString());
    ui->cbOriginalFilename->setChecked(settings->value("options/default_original_filename", false).toBool());
}

void UIThreadAdder::selectShortcutIndex(int idx) {
    selectShortcut(ui->cbFolderShortcuts->itemText(idx));
}

void UIThreadAdder::selectShortcut(QString name) {
    QString path;

    if (path != "-----") {
        path = folderShortcuts->getPath(name);

        if (!path.isEmpty())
            ui->leSavepath->setText(path);
    }
}

void UIThreadAdder::fillShortcutComboBox() {
    ui->cbFolderShortcuts->clear();
    ui->cbFolderShortcuts->addItem("-----");
    ui->cbFolderShortcuts->addItems(folderShortcuts->shortcuts());
}
