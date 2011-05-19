#include "charinfowidget.h"
#include "ui_charinfowidget.h"

CharInfoWidget::CharInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharInfoWidget)
{
    ui->setupUi(this);
}

CharInfoWidget::~CharInfoWidget()
{
    delete ui;
}

void CharInfoWidget::setCharInfo(CharInfo *c)
{
    m_charInfo = c;

    ui->spinX->setValue(c->rect.x());
    ui->spinY->setValue(c->rect.y());
    ui->spinW->setValue(c->rect.width());
    ui->spinH->setValue(c->rect.height());

    ui->lineChar->setText(c->ch);
    ui->checkBox->setChecked(c->ignore);

    ui->comboPosition->setCurrentIndex(qMax(0, (int)c->position));
    ui->comboCases->setCurrentIndex((c->chCase==CharInfo::TwoCases) ? 0 : 1);
}

void CharInfoWidget::on_pushSave_clicked()
{
    m_charInfo->rect.setX(ui->spinX->value());
    m_charInfo->rect.setY(ui->spinY->value());
    m_charInfo->rect.setWidth(ui->spinW->value());
    m_charInfo->rect.setHeight(ui->spinH->value());

    m_charInfo->ch = ui->lineChar->text().at(0);
    m_charInfo->ignore = ui->checkBox->isChecked();

    m_charInfo->position = (CharInfo::PostionInString) ui->comboPosition->currentIndex();
    m_charInfo->chCase =  ui->comboCases->currentIndex() == 0 ? CharInfo::TwoCases : CharInfo::FourCases;
}
