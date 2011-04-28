#ifndef UICONFIG_H
#define UICONFIG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include "uilisteditor.h"

namespace Ui {
    class UIConfig;
}

class UIConfig : public QDialog
{
    Q_OBJECT

public:
    explicit UIConfig(QWidget *parent = 0);
    ~UIConfig();

private:
    Ui::UIConfig *ui;
    QSettings* settings;
    UIListEditor* timeoutValueEditor;

private slots:
    void accept(void);
    void reject(void);
    void chooseLocation(void);
    void editTimeoutValues(void);
    void loadSettings(void);
    void toggleProxy(bool);

signals:
    void configurationChanged(void);
};

#endif // UICONFIG_H
