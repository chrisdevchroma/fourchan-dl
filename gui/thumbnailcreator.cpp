#include "thumbnailcreator.h"

ThumbnailCreator::ThumbnailCreator(QObject *parent) :
    QObject(parent)
{
    iconSize = new QSize(100,100);
    hq = true;
    newImages = false;
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    halted = false;
    use_combined_thumbnail_list = false;

    event_emit_timer = new QTimer(this);

    if (use_combined_thumbnail_list) {
        event_emit_timer->setInterval(2000);
        event_emit_timer->setSingleShot(true);

        connect(event_emit_timer, SIGNAL(timeout()), this, SLOT(eventEmitTimerTriggered()));
    }
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
            bool image_loaded;

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
            image_loaded = false;
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
                if (original.load(currentFilename)) {
                    image_loaded = true;
                }
                else {
                    if (QFile::exists(currentFilename)) {
                        QLOG_DEBUG() << __PRETTY_FUNCTION__ << ":: Image" << currentFilename << "cannot be processed. Using default thumbnail instead.";
                        if (original.load(":/icons/resources/image-missing.png")) {
                            image_loaded = true;

                            QLOG_TRACE() << __PRETTY_FUNCTION__ << ":: Loaded default image";
                        }
                    }
                    else {
                        QLOG_ERROR() << __PRETTY_FUNCTION__ << ":: Image" << currentFilename << " does not exist. Thumbnail not created.";
                        image_loaded = false;
                    }
                }

                if (image_loaded) {
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
            }

            mutex.lock();
            if (use_combined_thumbnail_list) {
                rendered_thumbnails << QString("%1:::%2").arg(currentFilename, cacheFile);
                if (!event_emit_timer->isActive()) {
                    event_emit_timer->start();
                }
            }
            else {
//                emit thumbnailAvailable(currentFilename, cacheFile);
            }
            emit pendingThumbnails(list.count());
            newImages = false;

            if (list.count() == 0) {
                if (use_combined_thumbnail_list) {
                    eventEmitTimerTriggered();
                }
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

QString ThumbnailCreator::addToList(QString s) {
    QString ret;

    ret = "";
    mutex.lock();
    if (!list.contains(s)) {
        ret = getCacheFile(s);
        list.append(s);
        newImages = true;
    }
    mutex.unlock();
    condition.wakeAll();

    return ret;
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
/*
void ThumbnailCreator::thumbnailAvailable(QString image_filename, QString cache_filename) {
//    mutex.lock();
    rendered_thumbnails.append(QString("%1:::%2").arg(image_filename, cache_filename));
    QLOG_TRACE() << __PRETTY_FUNCTION__ << ":: adding to list: " << rendered_thumbnails;
    thumbnails_created = true;

    if (!event_emit_timer->isActive()) {
        event_emit_timer->start();
    }
//    mutex.unlock();
}
*/
void ThumbnailCreator::eventEmitTimerTriggered() {
    mutex.lock();
    QLOG_TRACE() << __PRETTY_FUNCTION__ << ":: triggered";
    if (thumbnails_created) {
        thumbnails_created=false;
        QLOG_TRACE() << __PRETTY_FUNCTION__ << "emitting signal";
        QLOG_ALWAYS() << __PRETTY_FUNCTION__ << ":: Thumbnails ready: " << rendered_thumbnails;
//        emit thumbnailsAvailable(rendered_thumbnails.join(";;;"));
        rendered_thumbnails.clear();
    }
    mutex.unlock();
}

void ThumbnailCreator::wakeup() {
    condition.wakeAll();
}
