#include "uiinfo.h"
#include "ui_uiinfo.h"
UIInfo::UIInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIInfo)
{
    QString text;

    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    text = ui->label_2->text();
    text.replace("%%VERSION%%", PROGRAM_VERSION);
    text.replace("%%BUILDDATE%%", BUILDDATE);
    text.replace("%%QT_VERSION%%", QT_VERSION_STR);
    ui->label_2->setText(text);

    manager->get(QNetworkRequest(QUrl("http://sourceforge.net/projects/fourchan-dl/files/")));
}


void UIInfo::replyFinished(QNetworkReply* r) {
    QString requestURI;
    QRegExp rx("Current version ([0-9\\.]+)", Qt::CaseInsensitive, QRegExp::RegExp2);
    QString html;
    QString text;
    QStringList res;
    int pos;

    if (r->isFinished()) {
        requestURI = r->request().url().toString();

        html = r->readAll();

        pos = rx.indexIn(html);
        res = rx.capturedTexts();

        if (pos != -1) {
            text = ui->label_2->text();
            text.replace("%%CURRENT_VERSION%%", res.at(1));
            ui->label_2->setText(text);

            checkVersion(res.at(1));
        }

    }

    r->deleteLater();
}

void UIInfo::checkVersion(QString ver) {
    QStringList currVersion, thisVersion;

    currVersion = ver.split(".");
    thisVersion = QString(PROGRAM_VERSION).split(".");

    for (int i=0; i<currVersion.count(); i++) {
        if (currVersion.value(i).toInt() > thisVersion.at(i).toInt()) {
            emit newerVersionAvailable(ver);
            break;
        }
    }
}

UIInfo::~UIInfo()
{
    delete ui;
}
