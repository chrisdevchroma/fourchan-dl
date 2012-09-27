#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <QDialog>

namespace Ui {
    class ImagePreview;
}

class ImagePreview : public QDialog
{
    Q_OBJECT

public:
    explicit ImagePreview(QWidget *parent = 0);
    ~ImagePreview();

private slots:
    void next();
    void prev();
    void jump(int);

private:
    Ui::ImagePreview *ui;
};

#endif // IMAGEPREVIEW_H
