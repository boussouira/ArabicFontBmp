#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qcolordialog.h>
#include <qdebug.h>
#include <QSettings>
#include <QDial>
#include <QTextEdit>
#include <QFileDialog>

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

    generateText();
    ui->checkBox->setChecked(true);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings s;
    ui->lineOutputFile->setText(s.value("lastSaveFile").toString());
    m_font.fromString(s.value("font").toString());
    m_fontChanged = true;

    connect(ui->pushChangeFont, SIGNAL(clicked()), SLOT(changeFont()));
    connect(ui->pushGenerateImage, SIGNAL(clicked()), SLOT(generateImage()));
    connect(ui->pushCleanImage, SIGNAL(clicked()), SLOT(cleanImage()));
    connect(ui->pushCharMap, SIGNAL(clicked()), SLOT(viewCharMap()));
    connect(ui->pushGenCode, SIGNAL(clicked()), SLOT(generateCode()));
    connect(ui->pushSaveImage, SIGNAL(clicked()), SLOT(saveImage()));
    connect(ui->pushFgColor, SIGNAL(clicked()), SLOT(selectFgColor()));
    connect(ui->pushBgColor, SIGNAL(clicked()), SLOT(selectBgColor()));
    connect(ui->pushSaveFile, SIGNAL(clicked()), SLOT(selectOutFile()));
}

MainWindow::~MainWindow()
{
    QSettings s;
    s.setValue("font", m_font.toString());
    s.setValue("lastSaveFile", ui->lineOutputFile->text());

    delete ui;
}

void MainWindow::changeFont()
{
    m_font = QFontDialog::getFont(0, m_font, this);
    m_fontChanged = true;
    generateOptions();
}

void MainWindow::generateOptions()
{
    QFontMetrics fontInfo(m_font);
    int textH = fontInfo.height();
    int textW = fontInfo.width(ui->lineArabic->text()) + fontInfo.width(ui->lineSymbols->text());

    ui->spinImageH->setValue(textH);
    ui->spinImageW->setValue(textW);

    ui->spinDrawX->setValue(0);
    ui->spinDrawY->setValue(fontInfo.lineSpacing()-fontInfo.descent());
}

void MainWindow::generateImage()
{
    QString arText = ui->lineArabic->text();
    QString symbolText = ui->lineSymbols->text();
    QFontMetrics fontInfo(m_font);

    int textH = ui->spinImageH->value();
    int textW = ui->spinImageW->value();
    int symbolTextWidth = fontInfo.width(ui->lineSymbols->text());

    QPixmap temp = QPixmap(symbolTextWidth, textH);
    QPainter pTemp(&temp);
    pTemp.setRenderHint(QPainter::Antialiasing);
    pTemp.setRenderHint(QPainter::TextAntialiasing);
    pTemp.setRenderHint(QPainter::HighQualityAntialiasing);
    pTemp.setRenderHint(QPainter::SmoothPixmapTransform);

    pTemp.fillRect(0,0, temp.width(), temp.height(), QBrush(m_bgColor));
    pTemp.setPen(m_fgColor);
    pTemp.setFont(m_font);
    pTemp.drawText(ui->spinDrawX->value(), ui->spinDrawY->value(), symbolText);


    m_pixMap = QPixmap(textW, textH);

    QPainter p(&m_pixMap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    p.fillRect(0,0, m_pixMap.width(), m_pixMap.height(), QBrush(m_bgColor));
    p.setPen(m_fgColor);
    p.setFont(m_font);
    p.drawPixmap(0, 0, temp);
    p.drawText(ui->spinDrawX->value()+symbolTextWidth, ui->spinDrawY->value(), arText);

    ui->labelPreview->setPixmap(m_pixMap);
    if(m_fontChanged) {
        generateInfoList();
        m_fontChanged = false;
    }
}

void MainWindow::viewCharMap()
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
    QString arText = ui->lineArabic->text();
    QString symbolText = ui->lineSymbols->text();
    QFontMetrics fontInfo(m_font);
    QRect rect;
    int sx = 0;

    m_fullList.clear();

    for(int i=0; i<symbolText.size();i++) {
//        qDebug() << "Char:" << symbolText.at(i) << "at:" << i;
        CharInfo *info = new CharInfo;
        int w = fontInfo.width(symbolText.at(i));
        rect.setRect(sx, 0, w, ui->spinImageH->value());
        sx += w;

        info->rect = rect;
        info->ch = symbolText.at(i);

        m_fullList << info;
    }

    int ax = sx;
    for(int i=arText.size()-1; i>=0;i--) {
        //qDebug() << "Char:" << text.at(i) << "at:" << i;
        CharInfo *info = new CharInfo;
        int w = fontInfo.charWidth(arText, i);
        rect.setRect(ax, 0, w, ui->spinImageH->value());
        ax += w;
        info->rect = rect;
        info->ch = arText.at(i);

        QChar prevC;
        if(i-1 >= 0)
            prevC = arText.at(i-1);
        else
            prevC = 0;
        QChar currentC = arText.at(i);
        QChar nextC;
        if(i+1 < arText.size())
            nextC = arText.at(i+1);
        else
            nextC=0;

        if(currentC == 0x640 && i != arText.size()-1) {
            info->ignore = true;
        }

        if(currentC == 0x20) {
            info->ignore = true;
        }

        if(m_fourCases.contains(currentC.unicode())) {
            info->chCase = CharInfo::FourCases;

            if(currentC.unicode() == 0x644 && (nextC.unicode() == 0x627
                                               || nextC.unicode() == 0x622
                                               || nextC.unicode() == 0x623
                                               || nextC.unicode() == 0x625)) {
                info->composed = true;

                if(nextC.unicode() == 0x627)
                    info->composedType =  CharInfo::LamAlef;
                else if(nextC.unicode() == 0x623)
                    info->composedType =  CharInfo::LamAlefHamezaAbove;
                else if(nextC.unicode() == 0x625)
                    info->composedType =  CharInfo::LamAlefHamezaBelow;
                else if(nextC.unicode() == 0x622)
                    info->composedType =  CharInfo::LamAlefMada;

                if(prevC.isNull() || prevC.isSpace())
                    info->position = CharInfo::SINGLE;
                else
                    info->position = CharInfo::END;

                m_fullList << info;
                continue;
            }

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

            if(prevC.unicode() == 0x644 && (currentC.unicode() == 0x627
                                            || currentC.unicode() == 0x622
                                            || currentC.unicode() == 0x623
                                            || currentC.unicode() == 0x625)) {
                info->ignore = true;
                info->composed = true;
            }
                info->chCase = CharInfo::TwoCases;

                if(prevC.isNull() || prevC.isSpace())
                    info->position = CharInfo::SINGLE;
                else
                    info->position = CharInfo::END;
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

void MainWindow::on_lineArabic_editingFinished()
{
    generateOptions();
    generateInfoList();
}

void MainWindow::saveImage()
{
    ui->labelPreview->pixmap()->save(ui->lineOutputFile->text() ,"PNG");
}

void MainWindow::generateText()
{
    QString arText;
    QString symbolsText;

    foreach(int i, m_twoCases) {
         arText.append(trUtf8("%1 ـ%1").arg(QChar(i)));
         arText.append(" ");
    }

    foreach(int i, m_fourCases) {
        arText.append(trUtf8("%1" "ــــ"
                           "%1" "ــــ"
                           "%1 %1").arg(QChar(i)));
        arText.append(" ");
    }

    arText.append(trUtf8("لا ـلا لأ ـلأ لآ ـلآ لإ ـلإ"));
    arText.append(trUtf8("،,’؟"));
    arText.append(trUtf8("ـ"));

    symbolsText.append(trUtf8(" !\"#$%&'()*+,-./0123456789:;<=>?@"
//                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "[\\]^_`"
//                              "abcdefghijklmnopqrstuvwxyz"
                              "{|}~"
                              "¦«»"));

    ui->lineArabic->setText(arText);
    ui->lineSymbols->setText(symbolsText);
}

void MainWindow::selectFgColor()
{
    m_fgColor = QColorDialog::getColor(m_fgColor, this);
}

void MainWindow::selectBgColor()
{
    m_bgColor = QColorDialog::getColor(m_bgColor, this);
}

void MainWindow::cleanImage()
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

void MainWindow::generateCode()
{
    generateCleanList();

    QTextEdit *edit = new QTextEdit(0);

    JGenerator generator;
    qDebug() << "Start with" << m_cleanList.count() << "Chars";
    foreach(CharInfo *info, m_cleanList) {
        ushort ch = info->ch.unicode();

        if(info->widthAdded)
            continue;

        if(!info->composed) {
            if(JGenerator::isArabicChar(ch)) {
                qDebug() << "Add arabic" << info->ch;
                generator.addArabic(ch, getCharInfo(ch));
            } else {
                qDebug() << "Add simple" << info->ch;
                generator.addSimple(ch, getCharInfo(ch).first());
            }
        } else {
            generator.addComposed(ch, info->composedType, getCharInfo(ch, info->composedType));
        }
    }

    edit->setText(generator.getFunctions().join("\n"));
    edit->show();
}


QList<CharInfo*> MainWindow::getCharInfo(ushort ch, CharInfo::Composed composed)
{
    QList<CharInfo*> list;
    foreach(CharInfo *ci, m_cleanList) {
        if(ci->ch.unicode() == ch && ci->composed && ci->composedType == composed) {
            ci->widthAdded = true;
            list << ci;
        }
    }

    if(list.isEmpty()){
        qDebug() << "Got nothing for:" << ch << ":" << composed;
    } else {
        qDebug() << "Got for a composed:" << list.count();
    }

    return list;
}

QList<CharInfo*> MainWindow::getCharInfo(ushort ch)
{
    QList<CharInfo*> list;
    foreach(CharInfo *ci, m_cleanList) {
        if(ci->ch.unicode() == ch && !ci->composed) {
            ci->widthAdded = true;
            list.append(ci);
        }
    }

    qDebug() << "Got:" << list.count() << "for:" << "0x"+QString::number(ch, 16);
    return list;
}

void MainWindow::selectOutFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    ui->lineOutputFile->text(),
                                                    tr("PNG image (*.png)"));
    if(!fileName.isEmpty()) {
        ui->lineOutputFile->setText(fileName);
    }
}
