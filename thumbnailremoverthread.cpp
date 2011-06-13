#include "thumbnailremoverthread.h"

ThumbnailRemoverThread::ThumbnailRemoverThread(QObject *parent) :
    QThread(parent)
{
    settings = new QSettings("settings.ini", QSettings::IniFormat);
}

void ThumbnailRemoverThread::run() {
    QDateTime date;
    QFile f;
    mutex.lock();
    dirName = settings->value("options/thumbnail_cache_folder", QString("%1/%2").arg(QCoreApplication::applicationDirPath())
                          .arg("tncache")).toString();
    ttl = settings->value("options/thumbnail_TTL", 60).toInt();
    mutex.unlock();

    dir.setPath(dirName);
    fileInfoList = dir.entryInfoList();

    foreach (QFileInfo fi, fileInfoList) {
        date = fi.lastModified();
        if (date.addDays(ttl) < QDateTime::currentDateTime()) {
            f.remove(fi.absoluteFilePath());
            qDebug() << "removed " << fi.absoluteFilePath();
            qDebug() << " because it was created on " << date;
        }
    }
}
