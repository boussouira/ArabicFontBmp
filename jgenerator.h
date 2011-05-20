#ifndef JGENERATOR_H
#define JGENERATOR_H

#include <qobject.h>
#include <qstringlist.h>
#include "charinfo.h"

class JGenerator
{
public:
    JGenerator();
    void init();
    int getCharCases(ushort ch);
    static bool isArabicChar(ushort ch);
    void addArabic(ushort ch, QList<CharInfo *> info);
    void addSimple(ushort ch, CharInfo *info);
    void addComposed(ushort ch, CharInfo::Composed composed, QList<CharInfo *> info);
    QStringList getFunctions();

protected:
    void addTwoCases(ushort ch, CharInfo *endInfo, CharInfo *singleInfo);
    void addFourCases(ushort ch, CharInfo *startInfo, CharInfo *middleInfo, CharInfo *endInfo, CharInfo *singleInfo);
    void addComposedTwoCases(ushort ch, ushort nextC, CharInfo *endInfo, CharInfo *singleInfo);

private:
    QString _charWidth;
    QString _charX;
    QString _composeCharWidth;
    QString _composeCharX;
    QList<ushort> m_twoCases;
    QList<ushort> m_fourCases;
};

#endif // JGENERATOR_H
