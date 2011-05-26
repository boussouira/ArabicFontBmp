#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qfont.h>
#include <QFontDialog>
#include <charinfo.h>
#include "charinfowidget.h"
#include "jgenerator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void generateOptions();
    void generateInfoList();
    void generateCleanList();
    QList<CharInfo*> getCharInfo(ushort ch);
    QList<CharInfo*> getCharInfo(ushort ch, CharInfo::Composed composed);

private slots:
    void changeFont();
    void generateImage();
    void viewCharMap();
    void on_lineArabic_editingFinished();
    void saveImage();
    void generateText();
    void selectFgColor();
    void selectBgColor();
    QImage cleanImage();
    void generateCode();
    void selectOutFile();
    void charInfoChanged(int index);
    void nextCharInfo();
    void prevCharInfo();

    void on_checkAntiAlaise_toggled(bool checked);

private:
    QFont m_font;
    QImage m_pixMap;
    QColor m_fgColor;
    QColor m_bgColor;
    CharInfoWidget *m_charWidget;
    QList<ushort> m_twoCases;
    QList<ushort> m_fourCases;
    QList<ushort> m_ignoreChars;
    QList<CharInfo*> m_fullList;
    QList<CharInfo*> m_cleanList;
    bool m_fontChanged;
    bool m_antiAliase;
    int m_currentCharInfo;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
