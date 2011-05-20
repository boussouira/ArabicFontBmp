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
    int getCharX(CharInfo *ch, int w);
    int getCharsWidth();

    CharInfo * getCharInfo(ushort ch, CharInfo::Composed composed);
    int getCharX(ushort ch, int w, CharInfo::Composed composed);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_lineEdit_editingFinished();
    void on_pushButton_4_clicked();
    void on_pushSaveFile_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();


    void on_pushGenCode_clicked();

private:
    QFont m_font;
    QPixmap m_pixMap;
    QColor m_fgColor;
    QColor m_bgColor;
    CharInfoWidget *m_charWidget;
//    QList<CharInfo*> m_cleanCharList;
    QList<ushort> m_twoCases;
    QList<ushort> m_fourCases;
    QList<ushort> m_ignoreChars;
    QList<CharInfo*> m_fullList;
    QList<CharInfo*> m_cleanList;
    bool m_fontChanged;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
