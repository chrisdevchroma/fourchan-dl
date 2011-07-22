#ifndef UITHREADADDER_H
#define UITHREADADDER_H

#include <QDialog>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QtDebug>

namespace Ui {
    class UIThreadAdder;
}

class UIThreadAdder : public QDialog
{
    Q_OBJECT

public:
    explicit UIThreadAdder(QWidget *parent = 0);
    ~UIThreadAdder();

private:
    Ui::UIThreadAdder *ui;
    QClipboard* clipboard;

    void addLinks(QStringList links);
    QStringList parseText(QString text);
    QStringList parseHTML(QString html);
    QStringList parseURLList(QList<QUrl> urls);

private slots:
    void checkClipboard();
    void selectAll();
    void selectNone();

};

#endif // UITHREADADDER_H
