#include "thumbnailthread.h"

ThumbnailThread::ThumbnailThread(QObject *parent) :
    QThread(parent)
{
    iconSize = new QSize(100,100);
    hq = true;
    newImages = false;
    settings = new QSettings("settings.ini", QSettings::IniFormat);
}

void ThumbnailThread::createThumbnails() {
    QMutexLocker locker(&mutex);

    if (!isRunning()) {
        start(LowPriority);
    } else {
        condition.wakeOne();
    }
}

void ThumbnailThread::run() {
    QString currentFilename;
    int iconWidth;
    int iconHeight;
    bool enlargeThumbnails;
    bool hqRendering;

    forever {
        mutex.lock();
            if (list.count()>0) {
                currentFilename = list.front();
                list.pop_front();
                newImages = true;
            }
            iconWidth = iconSize->width();
            iconHeight = iconSize->height();
            enlargeThumbnails = settings->value("options/enlarge_thumbnails", false).toBool();
            hqRendering = settings->value("options/hq_thumbnails", false).toBool();
        mutex.unlock();

        if (newImages) {
            QImage original, tn;

            original.load(currentFilename);

            if (original.width()<iconWidth
                && original.height()<iconHeight
                && !(enlargeThumbnails)) {
                tn = original;
            } else {
                if (hqRendering)
                    tn = original.scaled(*iconSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
                else
                    tn = original.scaled(*iconSize,Qt::KeepAspectRatio,Qt::FastTransformation);
            }

            emit thumbnailCreated(currentFilename, tn);
            mutex.lock();
                emit pendingThumbnails(list.count());
                newImages = false;

                if (list.count() == 0)
                    condition.wait(&mutex);
            mutex.unlock();
        }
        else {
            mutex.lock();
                condition.wait(&mutex);
            mutex.unlock();
        }


    }
}

void ThumbnailThread::setIconSize(QSize s) {
    mutex.lock();
    iconSize->setWidth(s.width());
    iconSize->setHeight(s.height());
    mutex.unlock();
}

void ThumbnailThread::addToList(QString s) {
    mutex.lock();
    list.append(s);
    newImages = true;
    mutex.unlock();
}
