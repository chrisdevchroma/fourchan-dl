#include "updater.h"

Updater::Updater(QObject *parent) :
    QObject(parent)
{
    ai = new ApplicationInterface();
    dm = new DownloadManager();
    fh = new FileHandler();

    finishedDownload = false;
    exchanging = false;

    output = new QTextStream(stdout);
}

void Updater::run() {
    connect(ai, SIGNAL(startUpdate(QList<FileUpdate>)), this, SLOT(startUpdate(QList<FileUpdate>)));
    connect(dm, SIGNAL(downloadsFinished(QList<FileUpdate>)), this, SLOT(downloadFinished(QList<FileUpdate>)));
    connect(ai, SIGNAL(applicationClosed(bool)), this, SLOT(startExchange(bool)));
    connect(ai, SIGNAL(executableChanged(QString)), this, SLOT(setExecutable(QString)));
    connect(fh, SIGNAL(exchangingFinished()), this, SLOT(exchangeFinished()));
    connect(dm, SIGNAL(error(QString)), ai, SLOT(sendError(QString)));
    connect(fh, SIGNAL(error(QString)), ai, SLOT(sendError(QString)));

    connect(ai, SIGNAL(close()), this, SLOT(closeRequested()));
}

void Updater::startUpdate(QList<FileUpdate> l) {
    updateList = l;
    p("Starting Update");
    finishedDownload = false;

    dm->startDownload(l);
}

void Updater::downloadFinished(QList<FileUpdate> l) {
    updateList = l;
    finishedDownload = true;

    p("Downloads finished");

    ai->sendCloseRequest();
}

void Updater::startExchange(bool applicationClosed) {
    if (applicationClosed && finishedDownload && !exchanging) {
        p("Starting file exchange");
        exchanging = true;
        fh->startExchange(updateList);
    }
}

void Updater::exchangeFinished() {
    p("Update process finished");
    p("Starting Executable file "+executable);
    if (executable != "") {
        QProcess::startDetached(executable);
    }

    ai->sendMessage("Update finished.");
}

void Updater::setExecutable(QString s) {
    QFile f;

    if (f.exists(s)) {
        executable = s;
        p("Executable set to "+s);
    }
    else
        p("Wanted to set Executable file '"+s+"', but it does not exist.");

}


void Updater::p(QString msg) {
    *output << "Updater: " << msg << endl;
    output->flush();
}

void Updater::closeRequested() {
    exit(0);
}
