#ifndef CHARINFOWIDGET_H
#define CHARINFOWIDGET_H

#include <QWidget>
#include "charinfo.h"

namespace Ui {
    class CharInfoWidget;
}

class CharInfoWidget : public QWidget
{
    Q_OBJECT

public:
    CharInfoWidget(QWidget *parent = 0);
    void setCharInfo(CharInfo *c);
    ~CharInfoWidget();

private slots:
    void on_pushSave_clicked();

private:
    CharInfo *m_charInfo;
    Ui::CharInfoWidget *ui;
};

#endif // CHARINFOWIDGET_H
