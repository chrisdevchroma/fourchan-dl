#ifndef UIIMAGEVIEWER_H
#define UIIMAGEVIEWER_H

#include <QMainWindow>
#include <QFile>
#include <QtDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QMovie>
#include <QTimer>
#include "QsLog.h"

namespace Ui {
class UIImageViewer;
}

class UIImageViewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UIImageViewer(QWidget *parent = 0);
    ~UIImageViewer();
    void setImageList(QStringList imageList);
    void setCurrentImage(int i);
    void setCurrentImage(QString filename);
    void saveSettings();

private:
    Ui::UIImageViewer *ui;
    QStringList imagesToDisplay;
    int currentImage;
    QPixmap originalPixmap;
    QPixmap transformedPixmap;
    QSettings* settings;
    int rotation;
    QTimer* slideshowTimer;
    bool runSlideshow;

    void loadImage(int);
    void loadSettings();
    void transformPixmap();

private slots:
    void fitImage();
    void displayNextImage();
    void displayPrevImage();
    void openWithExternalViewer();
    void rotateCW();
    void rotateCCW();
    void toggleSlideshow(bool);
    void setSlideshowTimeout(int);

};

#endif // UIIMAGEVIEWER_H
