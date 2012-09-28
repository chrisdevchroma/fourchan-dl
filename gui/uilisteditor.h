#ifndef UILISTEDITOR_H
#define UILISTEDITOR_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>

namespace Ui {
    class UIListEditor;
}

class UIListEditor : public QDialog
{
    Q_OBJECT

public:
    explicit UIListEditor(QWidget *parent = 0);
    ~UIListEditor();
    void loadSettings(void);

private:
    Ui::UIListEditor *ui;
    QSettings* settings;

    void saveSettings(void);

private slots:
    void addItem(void);
    void removeItem(void);
    void accept(void);
    void reject(void);
    void editItem(QListWidgetItem*);
    void sortList(void);

signals:
    void valuesChanged(void);
};

#endif // UILISTEDITOR_H
