#include "thumbnailcreator.h"

ThumbnailCreator::ThumbnailCreator(QObject *parent) :
    QObject(parent)
{
    iconSize = new QSize(100,100);
    hq = true;
    newImages = false;
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    halted = false;
}

void ThumbnailCreator::go() {
    QImage original, tn;
    QString cacheFile;
    QString currentFilename;
    QDir dir;
    int iconWidth;
    int iconHeight;
    bool enlargeThumbnails;
    bool hqRendering;

    forever {
        mutex.lock();
            if (list.count()>0 && !halted) {
                currentFilename = list.front();
                list.pop_front();
                newImages = true;
                canceled = false;
                cacheFile = getCacheFile(currentFilename);
            }
        mutex.unlock();

        if (newImages) {
            bool useCachedThumbnail;

            iconWidth = iconSize->width();
            iconHeight = iconSize->height();
            enlargeThumbnails = settings->value("options/enlarge_thumbnails", false).toBool();
            hqRendering = settings->value("options/hq_thumbnails", false).toBool();
            useCache = true;
            cacheFolder = settings->value("options/thumbnail_cache_folder", QString("%1/%2").arg(QCoreApplication::applicationDirPath())
                                          .arg("tncache")).toString();

    //        QLOG_ALWAYS() << "ThumbnailCreator :: Using thumbnail folder " << cacheFolder;
            if (useCache && !(dir.exists(cacheFolder))) {
                QLOG_TRACE() << "ThumbnailCreator :: Creating thumbnail cache folder " << cacheFolder;
                dir.mkpath(cacheFolder);
            }

            useCachedThumbnail = false;
            // Check if thumbnail exists
            if (useCache && QFile::exists(cacheFile)) {
                QLOG_TRACE() << "ThumbnailCreator :: Cached thumbnail available for " << currentFilename;
                tn.load(cacheFile);

                if (tn.width() == iconWidth || tn.height() == iconHeight) {
                    useCachedThumbnail = true;
                }
            }

            if (!useCachedThumbnail){
                QLOG_TRACE() << "ThumbnailCreator :: Creating new thumbnail for " << currentFilename;
                original.load(currentFilename);
                QLOG_TRACE() << "ThumbnailCreator :: Loaded original file " << currentFilename;
                if (original.width()<iconWidth
                    && original.height()<iconHeight
                    && !(enlargeThumbnails)) {
                    QLOG_TRACE() << "ThumbnailCreator :: Setting original as thumbnail";
                    tn = original;
                } else {
                    QLOG_TRACE() << "ThumbnailCreator :: Rendering thumbnail";

                    if (hqRendering) {
                        tn = original.scaled(*iconSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
                    }
                    else {
                        tn = original.scaled(*iconSize,Qt::KeepAspectRatio,Qt::FastTransformation);
                    }
                }

                tn.save(cacheFile, "PNG");
                QLOG_TRACE() << "ThumbnailCreator :: Saving thumbnail as " << cacheFile;
            }

            mutex.lock();
                emit thumbnailAvailable(currentFilename, cacheFile);
                emit pendingThumbnails(list.count());
                newImages = false;

                if (list.count() == 0) {
                    condition.wait(&mutex);
                }

            mutex.unlock();
        }
        else {
            mutex.lock();
                condition.wait(&mutex);
            mutex.unlock();
        }
    }
}

void ThumbnailCreator::setIconSize(QSize s) {
    mutex.lock();
    iconSize->setWidth(s.width());
    iconSize->setHeight(s.height());
    mutex.unlock();
}

void ThumbnailCreator::addToList(QString s) {
    mutex.lock();
    if (!list.contains(s)) {
        list.append(s);
    }
    newImages = true;
    mutex.unlock();
    condition.wakeAll();
}

QString ThumbnailCreator::getCacheFile(QString filename) {
    QString tmp, ret;

    tmp = filename;
    tmp.replace( QRegExp( "[" + QRegExp::escape( "\\/:*?\"<>|" ) + "]" ), QString( "_" ) );
    ret= QString("%1/%2.tn").arg(cacheFolder).arg(tmp);

    return ret;
}

void ThumbnailCreator::halt() {
    mutex.lock();
    halted = true;
    mutex.unlock();
}

void ThumbnailCreator::resume() {
    mutex.lock();
    halted = false;
    mutex.unlock();
}
