#include "charinfo.h"

CharInfo::CharInfo()
{
    ignore = false;
    widthAdded = false;
    xAdded = false;
    composed = false;
    position = UNKNOW;
    chCase = ZeroCase;
}

CharInfo::CharInfo(CharInfo *copy)
{
    rect = copy->rect;
    ch = copy->ch;
    firstCh = copy->firstCh;
    secondCh = copy->secondCh;
    position = copy->position;
    chCase = copy->chCase;
    composedType = copy->composedType;
    ignore = copy->ignore;
    composed = copy->composed;
    widthAdded = copy->widthAdded;
    xAdded = copy->xAdded;
}

int CharInfo::x()
{
    return rect.x();
}

int CharInfo::width()
{
    return rect.width();
}
