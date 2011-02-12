#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "ui4chan.h"
#include "uiinfo.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<UI4chan> widgetList;

private slots:
    int addTab(void);
    void closeTab(int);
    void displayError(QString);
    void changeTabTitle(QString);
    void showInfo(void);
};

#endif // MAINWINDOW_H
