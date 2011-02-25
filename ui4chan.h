#ifndef UI4CHAN_H
#define UI4CHAN_H

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
#include <QClipboard>
#include <QDesktopServices>
#include <QStyle>
#include "parser.h"

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

private:
    Ui::UI4chan *ui;
    Parser* p;
    QTimer* timer;
    QList<int> timeoutValues;
    QSize iconSize;
    QAction* deleteFileAction;
    QAction* reloadFileAction;
    QAction* openFileAction;

private slots:
    void on_listWidget_customContextMenuRequested(QPoint pos);
    void chooseLocation(void);
    void triggerRescan(void);
    void addThumbnail(QString);
    void downloadsFinished(void);
    void deleteFile(void);
    void reloadFile(void);
    void openFile(void);
    void errorHandler(int);
    void setTabTitle(QString);
    void labelDirectoryChanged(QString);

public slots:
    void start(void);
    void stop(void);

signals:
    void finished(void);
    void errorMessage(QString);
    void tabTitleChanged(UI4chan*,QString);
    void directoryChanged(QString);
};

#endif // UI4CHAN_H
