#ifndef UI4CHAN_H
#define UI4CHAN_H

#include <QWidget>
#include <QFileDialog>
#include <QMenu>
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
    int getFrequency(void);

private:
    Ui::UI4chan *ui;
    Parser* p;
    QTimer* timer;
    QList<int> timeoutValues;
    QSize iconSize;
    QAction* deleteFileAction;
    QAction* reloadFileAction;

private slots:
    void on_listWidget_customContextMenuRequested(QPoint pos);
    void chooseLocation(void);
    void triggerRescan(void);
    void addThumbnail(QString);
    void downloadsFinished(void);
    void deleteFile(void);
    void reloadFile(void);
    void errorHandler(int);

public slots:
    void start(void);
    void stop(void);

signals:
    void finished(void);
    void errorMessage(QString);
};

#endif // UI4CHAN_H
