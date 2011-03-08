#ifndef UICONFIG_H
#define UICONFIG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>

namespace Ui {
    class UIConfig;
}

class UIConfig : public QDialog
{
    Q_OBJECT

public:
    explicit UIConfig(QWidget *parent = 0);
    ~UIConfig();

    void setSettings(QSettings* s);

private:
    Ui::UIConfig *ui;
    QSettings* settings;

private slots:
    void accept(void);
    void reject(void);
    void chooseLocation(void);

signals:
    void configurationChanged(void);
};

#endif // UICONFIG_H
