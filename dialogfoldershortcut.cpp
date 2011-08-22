#include "dialogfoldershortcut.h"
#include "ui_dialogfoldershortcut.h"

DialogFolderShortcut::DialogFolderShortcut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFolderShortcut)
{
    ui->setupUi(this);

    connect(folderShortcuts, SIGNAL(shortcutsChanged()), this, SLOT(fillShortcutComboBox()));
    connect(ui->cbFolderShortcuts, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectShortcut(QString)));
    fillShortcutComboBox();
}

DialogFolderShortcut::~DialogFolderShortcut()
{
    delete ui;
}

void DialogFolderShortcut::clear() {
    ui->leFolder->setText("");
    ui->leShortcutName->setText("");
}

void DialogFolderShortcut::selectShortcutIndex(int idx) {
    selectShortcut(ui->cbFolderShortcuts->itemText(idx));
}

void DialogFolderShortcut::selectShortcut(QString name) {
    QString path;

    if (path != "-----") {
        path = folderShortcuts->getPath(name);

        if (!path.isEmpty())
            ui->leFolder->setText(path);
    }
}

void DialogFolderShortcut::fillShortcutComboBox() {
    ui->cbFolderShortcuts->clear();
    ui->cbFolderShortcuts->addItem("-----");
    ui->cbFolderShortcuts->addItems(folderShortcuts->shortcuts());
}

void DialogFolderShortcut::checkValues() {
    bool ok;
    QString errMsg;

    ok = true;
    if (ui->leShortcutName->text().isEmpty()) {
        ok = false;
        errMsg = tr("You have to specify a shortcut name\n");
    }

    if (ui->leFolder->text().isEmpty()) {
        ok = false;
        errMsg.append(tr("You have to choose a path\n"));
    }

    if (ok) {
        emit shortcutChanged(ui->leShortcutName->text(), ui->leFolder->text());
        accept();
    }
    else {
        QMessageBox::critical(this, "Could not save changes", errMsg, QMessageBox::Ok);
    }
}

void DialogFolderShortcut::accept() {
    hide();
}

void DialogFolderShortcut::reject() {
    hide();
    emit editCanceled();
}

void DialogFolderShortcut::edit(QString name) {
    if (folderShortcuts->shortcutExists(name)) {
        ui->leShortcutName->setText(name);
        ui->leFolder->setText(folderShortcuts->getPath(name));
    } else {
        clear();
    }
}

void DialogFolderShortcut::choosePath() {
    QString loc;

    loc = QFileDialog::getExistingDirectory(this, "Choose directory", ui->leFolder->text());

    if (loc != "") {
        if (loc.endsWith("\\"))
            loc.chop(1);
        ui->leFolder->setText(loc);
    }
}
