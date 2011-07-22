#ifndef UI4CHAN_H
#define UI4CHAN_H

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
#include <QClipboard>
#include <QDesktopServices>
#include <QStyle>
#include <QSettings>
#include <QImage>
#include <QCloseEvent>
#include "parser.h"
#include "thumbnailthread.h"
#include "blacklist.h"
#include "downloadmanager.h"

class ThumbnailThread;
extern ThumbnailThread* tnt;

namespace Ui {
    class UI4chan;
}

class UI4chan : public QWidget
{
    Q_OBJECT

public:
    explicit UI4chan(QWidget *parent = 0);
    ~UI4chan();
    QString getURI(void);
    void setDirectory(QString);
    QString getValues(void);
    void setValues(QString);
    bool setThumbnailSize(QSize s);
    void setMaxDownloads(int);
    void useOriginalFilenames(bool);
    void updateSettings(void);
    void setBlackList(BlackList* bl);
//    void setDownloadManager(DownloadManager*);
private:
    Ui::UI4chan *ui;
    Parser* p;
//    ThumbnailThread* tnt;
    QTimer* timer;
    QList<int> timeoutValues;
    QSize iconSize;
    QAction* deleteFileAction;
    QAction* reloadFileAction;
    QAction* openFileAction;
    QSettings* settings;
    QStringList pendingThumbnails;
    BlackList* blackList;

    bool thumbnailsizeLocked;
    bool running;
    bool closeWhenFinished;

    void loadSettings(void);

private slots:
    void on_listWidget_customContextMenuRequested(QPoint pos);
    void chooseLocation(void);
    void triggerRescan(void);
    void createThumbnail(QString);
    void downloadsFinished(void);
    void deleteFile(void);
    void reloadFile(void);
    void openFile(void);
    void errorHandler(int);
    void messageHandler(QString);
    void setTabTitle(QString);
    void setMaxImageCount(int);
    void debugButton(void);
    void setDownloadedCount(int);
    void setPendingThumbnails(int);
    void processCloseRequest();
    void openURI();
    void openDownloadFolder();

public slots:
    void addThumbnail(QString, QImage);
    void start(void);
    void stop(void);

signals:
    void finished(void);
    void errorMessage(QString);
    void tabTitleChanged(UI4chan*,QString);
    void directoryChanged(QString);
    void closeRequest(UI4chan*, int);
    void createTabRequest(QString);
    void removeFiles(QStringList);
protected:
    void closeEvent(QCloseEvent *);
//    void keyPressEvent(QKeyEvent* ke);
};

#endif // UI4CHAN_H
