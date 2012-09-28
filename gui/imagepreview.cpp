#include "imagepreview.h"
#include "ui_imagepreview.h"

ImagePreview::ImagePreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImagePreview)
{
    ui->setupUi(this);
}

ImagePreview::~ImagePreview()
{
    delete ui;
}
