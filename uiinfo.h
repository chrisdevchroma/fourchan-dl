#ifndef UIINFO_H
#define UIINFO_H

#include <QDialog>

namespace Ui {
    class UIInfo;
}

class UIInfo : public QDialog
{
    Q_OBJECT

public:
    explicit UIInfo(QWidget *parent = 0);
    ~UIInfo();

private:
    Ui::UIInfo *ui;
};

#endif // UIINFO_H
