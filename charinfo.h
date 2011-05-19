#ifndef CHARINFO_H
#define CHARINFO_H

#include <qrect.h>
#include <qstring.h>

class CharInfo
{
public:
    enum PostionInString {
        UNKNOW = -1,
        START,
        MIDDLE,
        END,
        SINGLE
    };

    enum CharCase {
        ZeroCase,
        TwoCases = 2,
        FourCases = 4
    };

    enum Composed {
        LamAlef,
        LamAlefHamezaAbove,
        LamAlefHamezaBelow,
        LamAlefMada
    };

    CharInfo();
    QRect rect;
    QChar ch;
    QChar firstCh;
    QChar secondCh;
    PostionInString position;
    CharCase chCase;
    Composed composedType;
    bool ignore;
    bool composed;
    bool widthAdded;
    bool xAdded;
};

#endif // CHARINFO_H
