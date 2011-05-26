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

    init();
}

void JGenerator::init()
{
    _charWidth = "public int getCharWidth(char ch, char nextChar, char prevChar) {""\n"
            "int charWidth=0;""\n"
            "switch(ch) {""\n";

    _charX = "public int getCharX(char ch, char nextChar, char prevChar) {""\n"
            "int charX=0;""\n"
            "switch(ch) {""\n";

    _composeCharWidth =  "public int getComposedCharWidth(char ch, char nextChar, char prevChar) {""\n"
            "int charWidth=0;""\n";

    _composeCharX = "public int getComposedCharX(char ch, char nextChar, char prevChar) {""\n"
            "int charX=0;""\n";
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
        CharInfo *startInfo;
        CharInfo *middleInfo;
        CharInfo *endInfo;
        CharInfo *singleInfo;

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
}

void JGenerator::addSimple(ushort ch, CharInfo *info)
{
    _charWidth.append(QString("case 0x%1:""\n"
                              "charWidth = %2;""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(info->width()));

    _charX.append(QString("case 0x%1:""\n"
                          "charX = %2;""\n"
                          "break;""\n")
                  .arg(ch, 0, 16)
                  .arg(info->x()));
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
}

void JGenerator::addComposedTwoCases(ushort ch, ushort nextC, CharInfo *endInfo, CharInfo *singleInfo)
{
    _composeCharWidth.append(QString("if(ch == 0x%1 && nextChar == 0x%2){"
                                     "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                                     "charWidth = %3;""\n"
                                     "} else {""\n"
                                     "charWidth = %4;""\n"
                                     "}""\n"
                                     "}\n")
                             .arg(ch, 0, 16)
                             .arg(nextC, 0, 16)
                             .arg(singleInfo->width())
                             .arg(endInfo->width()));

    _composeCharX.append(QString("if(ch == 0x%1 && nextChar == 0x%2){"
                                 "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                                 "charX = %3;""\n"
                                 "} else {""\n"
                                 "charX = %4;""\n"
                                 "}""\n"
                                 "}\n")
                         .arg(ch, 0, 16)
                         .arg(nextC, 0, 16)
                         .arg(singleInfo->x())
                         .arg(endInfo->x()));
}

void JGenerator::addTwoCases(ushort ch, CharInfo *endInfo, CharInfo *singleInfo)
{
    _charWidth.append(QString("case 0x%1:""\n"
                              "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                              "charWidth = %2;""\n"
                              "} else {""\n"
                              "charWidth = %3;""\n"
                              "}""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(singleInfo->width())
                      .arg(endInfo->width()));

    _charX.append(QString("case 0x%1:""\n"
                          "if(!isArabicChar(prevChar) || getCharCases(prevChar) == 2) {""\n"
                          "charX = %2;""\n"
                          "} else {""\n"
                          "charX = %3;""\n"
                          "}""\n"
                          "break;""\n")
                  .arg(ch, 0, 16)
                  .arg(singleInfo->x())
                  .arg(endInfo->x()));
}

void JGenerator::addFourCases(ushort ch, CharInfo *startInfo, CharInfo *middleInfo, CharInfo *endInfo, CharInfo *singleInfo)
{
    if(ch == 0x644) {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if(nextChar == 0x627 || nextChar == 0x622 || nextChar == 0x623 || nextChar == 0x625){\n"
                                  "charWidth = getComposedCharWidth(ch, nextChar, prevChar);"
                                  "} else {\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "charWidth = %2;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "charWidth = %3;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "charWidth = %4;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "charWidth = %5;""\n"
                                  "}""\n"
                                  "}\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width()));
        _charX.append(QString("case 0x%1:""\n"
                              "if(nextChar == 0x627 || nextChar == 0x622 || nextChar == 0x623 || nextChar == 0x625){\n"
                              "charX = getComposedCharX(ch, nextChar, prevChar);"
                              "} else {\n"
                              "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                              "charX = %2;""\n"
                              "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                              "charX = %3;""\n"
                              "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                              "charX = %4;""\n"
                              "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                              "charX = %5;""\n"
                              "}""\n"
                              "}\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(startInfo->x())
                      .arg(middleInfo->x())
                      .arg(endInfo->x())
                      .arg(singleInfo->x()));
    } else if(ch == 0x64a) {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "charWidth = %2;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4 && nextChar != 0x621) {""\n"
                                  "charWidth = %3;""\n"
                                  "} else if((!isArabicChar(nextChar) && getCharCases(prevChar) == 4) || nextChar == 0x621) {""\n"
                                  "charWidth = %4;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "charWidth = %5;""\n"
                                  "}""\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width()));

        _charX.append(QString("case 0x%1:""\n"
                              "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                              "charX = %2;""\n"
                              "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4 && nextChar != 0x621) {""\n"
                              "charX = %3;""\n"
                              "} else if((!isArabicChar(nextChar) && getCharCases(prevChar) == 4) || nextChar == 0x621) {""\n"
                              "charX = %4;""\n"
                              "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                              "charX = %5;""\n"
                              "}""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(startInfo->x())
                      .arg(middleInfo->x())
                      .arg(endInfo->x())
                      .arg(singleInfo->x()));
    } else {
        _charWidth.append(QString("case 0x%1:""\n"
                                  "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                                  "charWidth = %2;""\n"
                                  "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "charWidth = %3;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                                  "charWidth = %4;""\n"
                                  "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                                  "charWidth = %5;""\n"
                                  "}""\n"
                                  "break;""\n")
                          .arg(ch, 0, 16)
                          .arg(startInfo->width())
                          .arg(middleInfo->width())
                          .arg(endInfo->width())
                          .arg(singleInfo->width()));

        _charX.append(QString("case 0x%1:""\n"
                              "if((!isArabicChar(prevChar) && !(getCharCases(prevChar) == 2)) || (isArabicChar(nextChar) &&  getCharCases(prevChar) == 2)) {""\n"
                              "charX = %2;""\n"
                              "} else if(isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                              "charX = %3;""\n"
                              "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 4) {""\n"
                              "charX = %4;""\n"
                              "} else if(!isArabicChar(nextChar) &&  getCharCases(prevChar) == 2) {""\n"
                              "charX = %5;""\n"
                              "}""\n"
                              "break;""\n")
                      .arg(ch, 0, 16)
                      .arg(startInfo->x())
                      .arg(middleInfo->x())
                      .arg(endInfo->x())
                      .arg(singleInfo->x()));
    }
}

QStringList JGenerator::getFunctions()
{
    _charWidth.append("default:\n"
                      "charWidth = 0;\n"
                      "}\n\n"
                      "return charWidth;\n"
                      "}\n");

    _charX.append("default:\n"
                  "charX = 0;\n"
                  "}\n\n"
                  "return charX;\n"
                  "}\n");

    _composeCharWidth.append("return charWidth;\n"
                             "}\n");
    _composeCharX.append("return charX;\n"
                             "}\n");
    QStringList functions;
    functions.append(_composeCharWidth);
    functions.append(_composeCharX);
    functions.append(_charWidth);
    functions.append(_charX);

    return functions;
}

