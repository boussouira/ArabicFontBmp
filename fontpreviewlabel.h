#ifndef FONTPREVIEWLABEL_H
#define FONTPREVIEWLABEL_H

#include <QLabel>
#include <charinfo.h>
#include <charinfowidget.h>

class FontPreviewLabel : public QLabel
{
    Q_OBJECT
public:
    FontPreviewLabel(QWidget *parent = 0);
    ~FontPreviewLabel();
    void setRectList(QList<CharInfo*> list);

protected:
    QList<CharInfo *> getRectList();
    void mousePressEvent(QMouseEvent *ev);

signals:
    void charInfoSelected(int index);

protected:
    QList<CharInfo*> m_charList;
};

#endif // FONTPREVIEWLABEL_H
