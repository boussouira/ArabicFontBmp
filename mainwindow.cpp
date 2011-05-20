#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qcolordialog.h>
#include <qdebug.h>
#include <QSettings>
#include <QDial>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_fgColor = QColor(Qt::black);
    m_bgColor = QColor(Qt::white);

    m_twoCases << 0x621 << 0x622 << 0x623 << 0x624 << 0x625
               << 0x627 << 0x629 << 0x62f << 0x630 << 0x631
               << 0x632 << 0x648 << 0x649;

    m_fourCases << 0x626 << 0x628 << 0x62a << 0x62b << 0x62c
                << 0x62d << 0x62e << 0x633 << 0x634 << 0x635
                << 0x636 << 0x637 << 0x638 << 0x639 << 0x63a
                << 0x641 << 0x642 << 0x643 << 0x644 << 0x645
                << 0x646 << 0x647 << 0x64a;

    m_charWidget = new CharInfoWidget(ui->widgetCharInfo);
    ui->labelPreview->setCharInfoWidget(m_charWidget);

    on_pushButton_5_clicked();
    ui->checkBox->setChecked(true);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings s;
    m_font.fromString(s.value("font").toString());
    m_fontChanged = true;
}

MainWindow::~MainWindow()
{
    QSettings s;
    s.setValue("font", m_font.toString());

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    m_font = QFontDialog::getFont(0, m_font, this);
    m_fontChanged = true;
    generateOptions();
}

void MainWindow::generateOptions()
{
    QFontMetrics fontInfo(m_font);
    int textH = fontInfo.height();
    int textW = fontInfo.width(ui->lineEdit->text());

    ui->spinImageH->setValue(textH);
    ui->spinImageW->setValue(textW);

    ui->spinDrawX->setValue(0);
    ui->spinDrawY->setValue(fontInfo.lineSpacing()-fontInfo.descent());
}

void MainWindow::on_pushButton_2_clicked()
{
    QString text = ui->lineEdit->text();
    QFontMetrics fontInfo(m_font);

    int textH = ui->spinImageH->value();
    int textW = ui->spinImageW->value();

    m_pixMap = QPixmap(textW, textH);

    QPainter p(&m_pixMap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(0,0, m_pixMap.width(), m_pixMap.height(), QBrush(m_bgColor));
    p.setPen(m_fgColor);
    p.setFont(m_font);
    p.drawText(ui->spinDrawX->value(), ui->spinDrawY->value(), text);

    int count=0;
    QString rendText;
    rendText.append(QString("public int charH = %1;\n").arg(textH));
    rendText.append("public int[] charW = {\n        ");
    for(int i=0; i<text.size();i++) {
        rendText.append(QString("%1").arg(fontInfo.charWidth(text, i)));

        if(i<text.size()-1)
            rendText.append(", ");

        if(++count >= 10) {
            rendText.append("\n        ");
            count = 0;
        }
    }
    rendText.append("};");

    rendText.append("\n\n/*\n");
    rendText.append(QString("Char count: %1\n").arg(text.count()));
    rendText.append(QString("Text height: %1").arg(textH));
    rendText.append("\n*/");

//    ui->plainTextEdit->setPlainText(rendText);
    ui->labelPreview->setPixmap(m_pixMap);
    if(m_fontChanged) {
        generateInfoList();
        m_fontChanged = false;
    }
//    qDebug() << "Leasing:" << fontInfo.leading();
//    qDebug() << "lineSpacing:" << fontInfo.lineSpacing();
//    qDebug() << "ascent:" << fontInfo.ascent();
//    qDebug() << "descent:" << fontInfo.descent();
}

void MainWindow::on_pushButton_3_clicked()
{
    QPixmap img(ui->spinImageW->value(), ui->spinImageH->value());
    QRect distRect;
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.setPen(Qt::red);

//    p.drawImage(0, 0, m_pixMap.toImage());

    distRect.setHeight(ui->spinImageH->value());
    distRect.setY(0);
    int dis_x = 0;

    foreach(CharInfo *ci, m_fullList) {
        distRect.setX(dis_x);
        distRect.setWidth(ci->rect.width());

        if(ci->ignore)
            p.setBrush(Qt::DiagCrossPattern);
        else
            p.setBrush(Qt::NoBrush);

        p.drawImage(distRect, m_pixMap.toImage(), ci->rect);
        p.drawRect(distRect);

        dis_x += ci->rect.width();
    }

    ui->labelPreview->setPixmap(img);
}

void MainWindow::generateInfoList()
{
    QString text = ui->lineEdit->text();
    QFontMetrics fontInfo(m_font);
    QRect rect;
    int cx = 0;

    m_fullList.clear();

    for(int i=text.size()-1; i>=0;i--) {
        //qDebug() << "Char:" << text.at(i) << "at:" << i;
        CharInfo *info = new CharInfo;
        int w = fontInfo.charWidth(text, i);
        rect.setRect(cx, 0, w, ui->spinImageH->value());
        cx += w;
        info->rect = rect;
        info->ch = text.at(i);

        QChar prevC;
        if(i-1 >= 0)
            prevC = text.at(i-1);
        else
            prevC = 0;
        QChar currentC = text.at(i);
        QChar nextC;
        if(i+1 < text.size())
            nextC = text.at(i+1);
        else
            nextC=0;

        if(currentC == 0x640) {
            if(m_ignoreChars.contains(0x640)) {
               info->ignore = true;
            } else {
                m_ignoreChars.append(0x640);
                qDebug("dont ignore at %d", i);
            }
        }

        if(currentC == 0x20) {
            if(m_ignoreChars.contains(0x20)) {
                info->ignore = true;
            } else {
                m_ignoreChars.append(0x20);
            }
        }

        if(m_fourCases.contains(currentC.unicode())) {
            info->chCase = CharInfo::FourCases;
/*
            if(currentC.unicode() == 0x644 && (nextC.unicode() == 0x627
                                               || nextC.unicode() == 0x622
                                               || nextC.unicode() == 0x623
                                               || nextC.unicode() == 0x625)) {
                info->composed = true;
                //info->composed = true;
                //info->composed = true;

                if(nextC.unicode() == 0x627)
                    info->composedType =  CharInfo::LamAlef;
                else if(nextC.unicode() == 0x623)
                    info->composedType =  CharInfo::LamAlefHamezaAbove;
                else if(nextC.unicode() == 0x625)
                    info->composedType =  CharInfo::LamAlefHamezaBelow;
                else if(nextC.unicode() == 0x622)
                    info->composedType =  CharInfo::LamAlefMada;

                //info->rect.setWidth(1000);
                //info->ch = currentC;
                charList << info;
                //i--;
                continue;
            }
*/
            if((prevC.isNull() || prevC.isSpace()) && !(nextC.isNull() || nextC.isSpace())) {
                info->position = CharInfo::START;
            } else if((nextC.isNull() || nextC.isSpace()) && !(prevC.isNull() || prevC.isSpace())) {
                info->position = CharInfo::END;
            } else if((nextC.isNull() || nextC.isSpace()) && (prevC.isNull() || prevC.isSpace())) {
                info->position = CharInfo::SINGLE;
            } else {
                info->position = CharInfo::MIDDLE;
            }
        } else if(m_twoCases.contains(currentC.unicode())) {
            /*
            if(prevC.unicode() == 0x644 && (currentC.unicode() == 0x627
                                            || currentC.unicode() == 0x622
                                            || currentC.unicode() == 0x623
                                            || currentC.unicode() == 0x625)) {
                info->widthAdded = true;
                info->xAdded = true;
                info->ignore = true;
                info->composed = true;
                //continue;
            } else {
            */
                info->chCase = CharInfo::TwoCases;

                if(prevC.isNull() || prevC.isSpace())
                    info->position = CharInfo::SINGLE;
                else
                    info->position = CharInfo::END;
            /*}*/
        }

        m_fullList << info;
    }

    generateCleanList();
    ui->labelPreview->setRectList(m_fullList);
}

void MainWindow::generateCleanList()
{
    int cx=0;

    qDeleteAll(m_cleanList);
    m_cleanList.clear();

    for(int i=0; i<m_fullList.size();i++) {
        if(m_fullList.at(i)->ignore)
            continue;

        CharInfo *info = new CharInfo(m_fullList.at(i));

        info->rect.moveLeft(cx);
        m_cleanList.append(info);

        cx += info->width();
    }
}

void MainWindow::on_lineEdit_editingFinished()
{
    generateOptions();
    generateInfoList();
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->labelPreview->pixmap()->save(ui->lineOutputFile->text() ,"PNG");
}

void MainWindow::on_pushSaveFile_clicked()
{

}

void MainWindow::on_pushButton_5_clicked()
{
    QString text;

    foreach(int i, m_twoCases) {
         text.append(trUtf8("%1 ـ%1").arg(QChar(i)));
         text.append(" ");
    }

    foreach(int i, m_fourCases) {
        text.append(trUtf8("%1" "ــــ"
                           "%1" "ــــ"
                           "%1 %1").arg(QChar(i)));
        text.append(" ");
    }

    //text.append(trUtf8("لا ـلا لأ ـلأ لآ ـلآ لإ ـلإ "));
    text.append(trUtf8("123456789[]{}+_)(*&^%$#@!~=-.<>,،ـ"));

    ui->lineEdit->setText(text);
    /*
    int count=0;
    for(int i=0x621; i<0x64B; i++) {
        if(0x63B <= i && i <= 0x640)
            continue;

        if(twoCases.contains(i)) {
            qDebug() << trUtf8("%1 ـ%1").arg(QChar(i));
        } else {
            qDebug() << trUtf8("%1%1%1").arg(QChar(i));
        }

//        qDebug() << tr("0x%1").arg(i,0, 16) << QChar(i);
        count++;
    }
    qDebug("Count: %d", count);
*/

}

void MainWindow::on_pushButton_6_clicked()
{
    m_fgColor = QColorDialog::getColor(m_fgColor, this);
}

void MainWindow::on_pushButton_7_clicked()
{
    m_bgColor = QColorDialog::getColor(m_bgColor, this);
}

void MainWindow::on_pushButton_8_clicked()
{
    int textH = ui->spinImageH->value();
    int textW = ui->spinImageW->value();

    QImage cleanImage(textW, textH, QImage::Format_RGB32);
    QPainter p(&cleanImage);
    QImage SourceImage = m_pixMap.toImage();

    QRect distRect;
    distRect.setHeight(textH);
    distRect.setY(0);

    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(0,0, cleanImage.width(), cleanImage.height(), QBrush(m_bgColor));
    p.setPen(m_fgColor);
    p.setFont(m_font);

    int dis_x = 0;

    for(int i=0; i<m_fullList.count();i++) {
        CharInfo *ci = m_fullList.at(i);

        if(ci->ignore)
            continue;

        distRect.setX(dis_x);
        distRect.setWidth(ci->rect.width());
        p.drawImage(distRect, SourceImage, ci->rect);

        dis_x += ci->rect.width();
    }

    QPixmap cleanPix(dis_x, cleanImage.height());
    QPainter px(&cleanPix);
    px.setRenderHint(QPainter::Antialiasing);
    px.setRenderHint(QPainter::TextAntialiasing);
    px.setRenderHint(QPainter::HighQualityAntialiasing);
    px.setRenderHint(QPainter::SmoothPixmapTransform);

    px.drawImage(0, 0, cleanImage, 0, 0, dis_x, cleanImage.height());

    ui->labelPreview->setPixmap(cleanPix);
}

void MainWindow::on_pushGenCode_clicked()
{
    generateCleanList();

    QString text;
    QTextEdit *edit = new QTextEdit(0);
    int totalCharsWidth = getCharsWidth();

    JGenerator generator;
    qDebug() << "Start with" << m_cleanList.count() << "Chars";
    foreach(CharInfo *info, m_cleanList) {
        ushort ch = info->ch.unicode();

        if(info->widthAdded)
            continue;

        if(JGenerator::isArabicChar(ch)) {
            qDebug() << "Add arabic" << info->ch;
            generator.addArabic(ch, getCharInfo(ch));
        } else {
            qDebug() << "Add simple" << info->ch;
            generator.addSimple(ch, getCharInfo(ch).first());
        }
    }

    edit->setText(generator.getFunctions().join("\n/*********/\n"));
    edit->show();
}


CharInfo *MainWindow::getCharInfo(ushort ch, CharInfo::Composed composed)
{
    CharInfo *charInfo = 0;
    foreach(CharInfo *ci, m_cleanList) {
        if(ci->ch.unicode() == ch && ci->composed && ci->composedType == composed) {
            ci->widthAdded = true;
            charInfo = ci;
//            qDebug() << "Got some for:" << ch;
        }
    }

    if(!ch){
        qDebug() << "Got nothing for:" << ch << ":" << composed;
        charInfo = new CharInfo();
    }

    return charInfo;
}

QList<CharInfo*> MainWindow::getCharInfo(ushort ch)
{
    QList<CharInfo*> list;
    foreach(CharInfo *ci, m_cleanList) {
        if(ci->ch.unicode() == ch) {
            ci->widthAdded = true;
            list.append(ci);
        }
    }

    qDebug() << "Got:" << list.count() << "for:" << "0x"+QString::number(ch, 16);
    return list;
}

int MainWindow::getCharsWidth()
{
    int dis_x = 0;
    for(int i=0; i<m_fullList.count();i++) {
        if(m_fullList.at(i)->ignore)
            continue;

        dis_x += m_fullList.at(i)->rect.width();
    }

    return dis_x;
}

int MainWindow::getCharX(CharInfo *ch, int w)
{
    for(int i=m_cleanList.count()-1; i>=0;i--) {
        CharInfo *ci = m_cleanList.at(i);
        if(ci->ignore)
            continue;

        w -= ci->rect.width();

        if(ci->ch == ch->ch && ch->position == ci->position)
            break;
    }
    ch->xAdded = true;
    //qDebug() << ch->ch << "==" << w;
    if(w<0)
        qDebug() << "********************* What **************";
    return w;
}

int MainWindow::getCharX(ushort ch, int w, CharInfo::Composed composed)
{
    for(int i=m_cleanList.count()-1; i>=0;i--) {
        CharInfo *ci = m_cleanList.at(i);
        if(ci->ignore)
            continue;

        w -= ci->rect.width();

        if(ci->ch == ch && ci->composedType == composed) {
         ci->xAdded = true;
            break;
        }
    }
//    ch->xAdded = true;
    //qDebug() << ch->ch << "==" << w;
    if(w<0)
        qDebug() << "********************* What **************";
    return w;
}
