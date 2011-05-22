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
    void save();

signals:
    void charInfoChanged();

private:
    CharInfo *m_charInfo;
    bool m_loadingCharInfo;
    Ui::CharInfoWidget *ui;
};

#endif // CHARINFOWIDGET_H
