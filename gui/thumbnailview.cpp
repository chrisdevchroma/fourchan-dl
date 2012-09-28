#include "thumbnailview.h"

ThumbnailView::ThumbnailView(QWidget *parent) :
    QListWidget(parent)
{
}

void ThumbnailView::keyPressEvent(QKeyEvent *event) {
    QListWidget::keyPressEvent(event);

    // Own key mappings
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit openItem();
        break;

    case Qt::Key_Delete:
        emit deleteItem();
        break;

    case Qt::Key_R:
        emit reloadItem();
        break;
    }
}
