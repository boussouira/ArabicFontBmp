#include "fontpreviewlabel.h"
#include <qdebug.h>
#include <QMouseEvent>
#include <qmenu.h>
#include <qaction.h>

FontPreviewLabel::FontPreviewLabel(QWidget *parent) :
    QLabel(parent)
{
}

FontPreviewLabel::~FontPreviewLabel()
{
}

void FontPreviewLabel::mousePressEvent(QMouseEvent *ev)
{
    //    qDebug() << ev->pos();
    for(int i=0; i<m_charList.count(); i++) {
        int x1 = m_charList.at(i)->rect.x();
        int x2 = m_charList.at(((i+1)<m_charList.count()) ? i+1 : i)->rect.x()+(((i+1)<m_charList.count()) ? 0 : m_charList.at(i)->rect.width());

        if(x1 < ev->pos().x() && ev->pos().x() < x2) {
            //qDebug() << m_charList.at(i)->ch << m_charList.at(i)->rect;
            CharInfo *charInfo = m_charList.at(i);

            if(ev->button() == Qt::RightButton) {
                QMenu menu(this);
                QAction *actIgnore = new QAction(tr("Ignore '%1'").arg(charInfo->ch), this);
                QAction *actIgnoreLikeThis = new QAction(tr("Ignore every '%1'").arg(charInfo->ch), this);

                menu.addAction(actIgnore);
                menu.addAction(actIgnoreLikeThis);

                QAction *ret = menu.exec(ev->globalPos());
                if(ret == actIgnore) {
                    charInfo->ignore = true;
                } else if (ret == actIgnoreLikeThis) {
                    for(int j=0; j<m_charList.count(); j++){
                        CharInfo *charI = m_charList.at(j);
                        if(charI->ch == charInfo->ch)
                            charI->ignore = true;
                    }
                }
            }

            emit charInfoSelected(i);
        }
    }
}

void FontPreviewLabel::setRectList(QList<CharInfo*> list)
{
    m_charList = list;
}

QList<CharInfo*> FontPreviewLabel::getRectList()
{
    return m_charList;
}
