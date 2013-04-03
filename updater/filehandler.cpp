#include "filehandler.h"

FileHandler::FileHandler(QObject *parent) :
    QObject(parent)
{
}

void FileHandler::startExchange(QList<FileUpdate> l) {
    //bool filesRemaining;
    bool errors;
    QString sourceFilename;
    QString targetFilename;
    FileUpdate fu;

    updateList = l;
    errors = false;

    //filesRemaining = false;

    for (int i=0; i<updateList.count(); i++) {
            sourceFilename = updateList.at(i).tmpFilename;
            targetFilename = updateList.at(i).filename;

        p("Exchanging "+sourceFilename+" with "+targetFilename);

        QFile source;
        QFile target;

        source.setFileName(sourceFilename);
        target.setFileName(targetFilename);

        source.open(QIODevice::ReadWrite);

        if (target.exists()) {
            if  (!target.remove()) {
                p("Cannot remove "+targetFilename);
                error("Cannot remove "+targetFilename+" ("+target.errorString()+")");

                fu.tmpFilename = sourceFilename;
                fu.filename = targetFilename;
                failedFiles.append(fu);
                errors = true;
            }
        }

        if (!source.rename(targetFilename)) {
            p("Could not rename "+sourceFilename+" to "+targetFilename);
            error("Could not rename "+sourceFilename+" to "+targetFilename+" ("+target.errorString()+")");

            fu.tmpFilename = sourceFilename;
            fu.filename = targetFilename;
            failedFiles.append(fu);

            errors = true;
        }

#ifdef Q_OS_LINUX
        // On Linux we have to set executable rights
        target.setPermissions(target.permissions() | QFile::ExeOther);
#endif

    }

    if (errors) p("Rename had errors");

    p("Updating finished - If this window doesn't close it save to close it manually.");
    emit exchangingFinished(errors);
}

void FileHandler::p(QString msg) {
    *output << "FileHandler: " << msg << endl;
    output->flush();
    *foutput << "FileHandler: " << msg << endl;
    foutput->flush();
}
