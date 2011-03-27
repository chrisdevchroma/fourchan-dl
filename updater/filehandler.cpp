#include "filehandler.h"

FileHandler::FileHandler(QObject *parent) :
    QObject(parent)
{
    output = new QTextStream(stdout);
}

void FileHandler::startExchange(QList<FileUpdate> l) {
    updateList = l;

    bool filesRemaining;
    QString sourceFilename;
    QString targetFilename;

    filesRemaining = false;

    for (int i=0; i<updateList.count(); i++) {
            sourceFilename = updateList.at(i).tmpFilename;
            targetFilename = updateList.at(i).filename;

        p("Exchanging "+sourceFilename+" with "+targetFilename);

        QFile source;
        QFile target;

        source.setFileName(sourceFilename);
        target.setFileName(targetFilename);

        source.open(QIODevice::ReadWrite);

        if (target.exists())
            if  (!target.remove()) {
                p("Cannot remove "+targetFilename);
                error("Cannot remove "+targetFilename+" ("+target.errorString()+")");
            }

        if (!source.rename(targetFilename)) {
            p("Could not rename "+sourceFilename+" to "+targetFilename);
            error("Could not rename "+sourceFilename+" to "+targetFilename+" ("+target.errorString()+")");
        }

#ifdef Q_OS_LINUX
        // On Linux we have to set executable rights
        target.setPermissions(target.permissions() | QFile::ExeOther);
#endif

    }

    p("Updating finished - If this window doesn't close it save to close it manually.");
    emit exchangingFinished();
}

void FileHandler::p(QString msg) {
    *output << "FileHandler: " << msg << endl;
    output->flush();
}
