#include "uithreadadder.h"
#include "ui_uithreadadder.h"

UIThreadAdder::UIThreadAdder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIThreadAdder)
{
    ui->setupUi(this);
    clipboard = QApplication::clipboard();

//    connect(clipboard, SIGNAL(changed(QClipboard::Mode))), this, SLOT(checkClipboard(QClipboard::Mode));
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(checkClipboard()));
    connect(ui->btnSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(ui->btnSelectNone, SIGNAL(clicked()), this, SLOT(selectNone()));
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
    ui->listWidget->clear();
//    switch(mode) {
//    case QClipboard::Clipboard:     // Look only in global clipboard
        if (clipboard->mimeData(mode)->hasText() || clipboard->mimeData(mode)->hasHtml()) {
            text = clipboard->mimeData(mode)->text();
            links = parseText(text);
        }
        else if (clipboard->mimeData(mode)->hasUrls()) {
            urls = clipboard->mimeData(mode)->urls();
        }
//        break;

//    default:
//        break;
//    }

        foreach(QString link, links) {
            QListWidgetItem* item;

            item = new QListWidgetItem(ui->listWidget);
            item->setText(link);
            item->setCheckState(Qt::Checked);
            ui->listWidget->addItem(item);
        }

        qDebug() << "finished";
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
//    QRegExp rx("(https?://)?(www\\.)?([a-zA-Z0-9_%-\.]*)\\b\\.[a-z]{2,4}(\\.[a-z]{2})?((/[a-zA-Z0-9_%-]*)+)?(ß\.[a-z]*)?", Qt::CaseInsensitive, QRegExp::RegExp2);
//    QRegExp rx("(?i)\\b((?:[a-z][\\w-]+:(?:/{1,3}|[a-z0-9%])|www\\d{0,3}[.]|[a-z0-9.\\-]+[.][a-z]{2,4}/)(?:[^\\s()<>]+|\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\))+(?:\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\)|[^\\s`!()\\[\\]{};:'\".,?]))", Qt::CaseInsensitive, QRegExp::RegExp);
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

        if ((url.startsWith("http") || url.startsWith("www")) && !url.contains("4chan.org"))
            url.clear();

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
