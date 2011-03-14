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
    forever {
        mutex.lock();
        QStringList sl = list;
        list.clear();
        int iconWidth = iconSize->width();
        int iconHeight = iconSize->height();
        bool enlargeThumbnails = settings->value("options/enlarge_thumbnails", false).toBool();
        bool hqRendering = settings->value("options/hq_thumbnails", false).toBool();
//        qDebug()<<"List size:" << sl.count();
        newImages = false;
        mutex.unlock();

        while (!sl.isEmpty()) {
            QImage original, tn;

            original.load(sl.at(0));

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

            emit thumbnailCreated(sl.at(0), tn);
            sl.pop_front();
        }

        mutex.lock();
        if (!newImages)
        condition.wait(&mutex);
        mutex.unlock();
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
