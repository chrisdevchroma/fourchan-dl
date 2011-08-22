#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QListWidget>
#include <QKeyEvent>
#include <QtDebug>
#include <QApplication>
#include <QClipboard>

class ThumbnailView : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailView(QWidget *parent = 0);
protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void openItem();
    void deleteItem();
    void reloadItem();
};

#endif // THUMBNAILVIEW_H
