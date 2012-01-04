#include "jgenerator.h"
#include <qdebug.h>

JGenerator::JGenerator()
{
    m_twoCases << 0x621 << 0x622 << 0x623 << 0x624 << 0x625
               << 0x627 << 0x629 << 0x62f << 0x630 << 0x631
               << 0x632 << 0x648 << 0x649;

    m_fourCases << 0x626 << 0x628 << 0x62a << 0x62b << 0x62c
                << 0x62d << 0x62e << 0x633 << 0x634 << 0x635
                << 0x636 << 0x637 << 0x638 << 0x639 << 0x63a
                << 0x641 << 0x642 << 0x643 << 0x644 << 0x645
                << 0x646 << 0x647 << 0x64a;

    m_maxCharWidth = 0;
    m_charHeight = 0;

    init();
}

void JGenerator::init()
{
    _charWidth = "public int getCharInfo(char ch, char nextChar, char prevChar, int info) {""\n"
            "int ret=0;""\n"
            "switch(ch) {""\n";

    _composeCharWidth =  "public int getComposedCharInfo(char ch, char nextChar, char prevChar, int info) {""\n"
            "int ret=0;""\n";
}

int JGenerator::getCharCases(ushort ch){
    int ret;
    switch(ch) {
    case 0x621:
    case 0x622:
    case 0x623:
    case 0x624:
    case 0x625:
    case 0x627:
    case 0x629:
    case 0x62f:
    case 0x630:
    case 0x631:
    case 0x632:
    case 0x648:
    case 0x649:
        ret = 2;
        break;

    case 0x626:
    case 0x628:
    case 0x62a:
    case 0x62b:
    case 0x62c:
    case 0x62d:
    case 0x62e:
    case 0x633:
    case 0x634:
    case 0x635:
    case 0x636:
    case 0x637:
    case 0x638:
    case 0x639:
    case 0x63a:
    case 0x641:
    case 0x642:
    case 0x643:
    case 0x644:
    case 0x645:
    case 0x646:
    case 0x647:
    case 0x64a:
        ret = 4;
        break;
    default:
        ret = 0;
    }

    return ret;
}

bool JGenerator::isArabicChar(ushort ch){
    return ((0x0621 <= ch &&  ch <= 0x064A) && ch != 0x640);
}

void JGenerator::addArabic(ushort ch, QList<CharInfo *> info)
{
    if(getCharCases(ch) == 2) {
        CharInfo *endInfo=0;
        CharInfo *singleInfo=0;

        foreach(CharInfo* ci, info) {
            if(ci->position == CharInfo::END)
                endInfo = ci;
            else if (ci->position == CharInfo::SINGLE)
                singleInfo = ci;
            else
                qDebug("Not handled: %d", ci->position);
        }

        addTwoCases(ch, endInfo, singleInfo);
    } else if (getCharCases(ch) == 4) {
        CharInfo *startInfo=0;
        CharInfo *middleInfo=0;
        CharInfo *endInfo=0;
        CharInfo *singleInfo=0;

        foreach(CharInfo* ci, info) {
            if(ci->position == CharInfo::START)
                startInfo = ci;
            else if (ci->position == CharInfo::MIDDLE)
                middleInfo = ci;
            else if (ci->position == CharInfo::END)
                endInfo = ci;
            else if (ci->position == CharInfo::SINGLE)
                singleInfo = ci;
            else
                qDebug("Not handled: %d", ci->position);

        }

        addFourCases(ch, startInfo, middleInfo, endInfo, singleInfo);
    } else {
        qDebug() << "Not handled:" << "0x" + QString::number(ch, 16);
    }

    maxChar(info);
}

void JGenerator::addSimple(ushort ch, CharInfo *info)
{
    _charWidth.append(QString("case 0x%1:""\n"
                              "ret =(info == CHAR_WIDTH) ? %2 : %3;""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(info->width())
                      .arg(info->x()));

    maxChar(info->width());
}


void JGenerator::addComposed(ushort ch, CharInfo::Composed composed, QList<CharInfo *> info)
{
    CharInfo *endInfo=0;
    CharInfo *singleInfo=0;
    ushort nextC;

    foreach(CharInfo* ci, info) {
        if(ci->position == CharInfo::END)
            endInfo = ci;
        else if (ci->position == CharInfo::SINGLE)
            singleInfo = ci;
        else
            qDebug("Not handled: %d", ci->position);
    }

    switch(composed) {
    case CharInfo::LamAlef:
        nextC = 0x0627;
        break;
    case CharInfo::LamAlefHamezaAbove:
        nextC = 0x0623;
        break;
    case CharInfo::LamAlefHamezaBelow:
        nextC = 0x0625;
        break;
    case CharInfo::LamAlefMada:
        nextC = 0x0622;
        break;
    default:
        nextC = 0;
        qDebug("Unknow Case..");
    }

    addComposedTwoCases(ch, nextC, endInfo, singleInfo);

    maxChar(info);
}

void JGenerator::addComposedTwoCases(ushort ch, ushort nextC, CharInfo *endInfo, CharInfo *singleInfo)
{
    _composeCharWidth.append(QString("if(ch == 0x%1 && nextChar == 0x%2){"
                                     "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                                     "ret =(info == CHAR_WIDTH) ? %3 : %5;""\n"
                                     "} else {""\n"
                                     "ret =(info == CHAR_WIDTH) ? %4 : %6;""\n"
                                     "}""\n"
                                     "}\n")
                             .arg(ch, 0, 16)
                             .arg(nextC, 0, 16)
                             .arg(singleInfo->width())
                             .arg(endInfo->width())
                             .arg(singleInfo->x())
                             .arg(endInfo->x()));

}

void JGenerator::addTwoCases(ushort ch, CharInfo *endInfo, CharInfo *singleInfo)
{
    _charWidth.append(QString("case 0x%1:""\n"
                              "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                              "ret =(info == CHAR_WIDTH) ? %2 : %4;""\n"
                              "} else {""\n"
                              "ret =(info == CHAR_WIDTH) ? %3 : %5;""\n"
                              "}""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(singleInfo->width())
                      .arg(endInfo->width())
                      .arg(singleInfo->x())
                      .arg(endInfo->x()));
}

void JGenerator::addFourCases(ushort ch, CharInfo *startInfo, CharInfo *middleInfo, CharInfo *endInfo, CharInfo *singleInfo)
{
    if(ch == 0x644) {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if(nextChar == 0x627 || nextChar == 0x622 || nextChar == 0x623 || nextChar == 0x625){\n"
                                  "ret = getComposedCharWidth(ch, nextChar, prevChar, info);"
                                  "} else {\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %2 : %6;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %3 : %7;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %4 : %8;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %5 : %9;""\n"
                                  "}""\n"
                                  "}\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width())
                          .arg(startInfo->x())
                          .arg(middleInfo->x())
                          .arg(endInfo->x())
                          .arg(singleInfo->x()));
    } else if(ch == 0x64a) {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %2 : %6;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4 && nextChar != 0x621) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %3 : %7;""\n"
                                  "} else if((!isArabicChar(nextChar) && getCharCases(prevChar) == 4) || nextChar == 0x621) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %4 : %8;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %5 : %9;""\n"
                                  "}""\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width())
                          .arg(startInfo->x())
                          .arg(middleInfo->x())
                          .arg(endInfo->x())
                          .arg(singleInfo->x()));

    } else {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %2 : %6;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %3 : %7;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %4 : %8;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "ret =(info == CHAR_WIDTH) ? %5 : %9;""\n"
                                  "}""\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width())
                          .arg(startInfo->x())
                          .arg(middleInfo->x())
                          .arg(endInfo->x())
                          .arg(singleInfo->x()));
    }
}

QStringList JGenerator::getFunctions()
{
    _charWidth.append("default:\n"
                      "ret = 0;\n"
                      "}\n\n"
                      "return ret;\n"
                      "}\n");

    _composeCharWidth.append("return ret;\n"
                             "}\n");

    QStringList functions;
//    functions.append(QString("final public int CHAR_WIDTH=1;"));
//    functions.append(QString("final public int CHAR_X=2;"));
    functions.append(QString("public int getCharHeight() {\n"
                             "return %1;\n"
                             "}").arg(m_charHeight));

    functions.append(QString("public int getMaxWidth() {\n"
                             "return %1;\n"
                             "}").arg(m_maxCharWidth));

    functions.append(_composeCharWidth);
    functions.append(_charWidth);

    return functions;
}

void JGenerator::maxChar(int width)
{
    if(m_maxCharWidth < width)
        m_maxCharWidth = width;
}

void JGenerator::maxChar(QList<CharInfo *> info)
{
    foreach(CharInfo* ci, info) {
        maxChar(ci->width());
    }
}

void JGenerator::setCharHeight(int h)
{
    m_charHeight = h;
}

