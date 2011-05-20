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
    void setCharInfoWidget(CharInfoWidget *w);

protected:
    QList<CharInfo *> getRectList();
    void mousePressEvent(QMouseEvent *ev);

protected:
    CharInfoWidget *m_charWidget;
     QList<CharInfo*> m_charList;
};

#endif // FONTPREVIEWLABEL_H
