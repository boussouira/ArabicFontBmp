#include "charinfowidget.h"
#include "ui_charinfowidget.h"

CharInfoWidget::CharInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharInfoWidget)
{
    ui->setupUi(this);
    m_loadingCharInfo = true;

    connect(ui->pushSave, SIGNAL(clicked()), SLOT(save()));
    ui->pushSave->hide();

    connect(ui->spinW, SIGNAL(valueChanged(int)), SLOT(save()));
    connect(ui->spinH, SIGNAL(valueChanged(int)), SLOT(save()));
    connect(ui->spinX, SIGNAL(valueChanged(int)), SLOT(save()));
    connect(ui->spinY, SIGNAL(valueChanged(int)), SLOT(save()));
    connect(ui->lineChar, SIGNAL(textChanged(QString)), SLOT(save()));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), SLOT(save()));
    connect(ui->comboPosition, SIGNAL(currentIndexChanged(int)), SLOT(save()));
    connect(ui->comboCases, SIGNAL(currentIndexChanged(int)), SLOT(save()));
}

CharInfoWidget::~CharInfoWidget()
{
    delete ui;
}

void CharInfoWidget::setCharInfo(CharInfo *c)
{
    m_charInfo = c;
    m_loadingCharInfo = true;

    ui->spinX->setValue(c->rect.x());
    ui->spinY->setValue(c->rect.y());
    ui->spinW->setValue(c->rect.width());
    ui->spinH->setValue(c->rect.height());

    ui->lineChar->setText(c->ch);
    ui->checkBox->setChecked(c->ignore);

    ui->comboPosition->setCurrentIndex(qMax(0, (int)c->position));
    ui->comboCases->setCurrentIndex((c->chCase==CharInfo::TwoCases) ? 0 : 1);

    m_loadingCharInfo = false;
}

void CharInfoWidget::save()
{
    if(!m_loadingCharInfo) {
        m_charInfo->rect.setX(ui->spinX->value());
        m_charInfo->rect.setY(ui->spinY->value());
        m_charInfo->rect.setWidth(ui->spinW->value());
        m_charInfo->rect.setHeight(ui->spinH->value());

        m_charInfo->ch = ui->lineChar->text().at(0);
        m_charInfo->ignore = ui->checkBox->isChecked();

        m_charInfo->position = (CharInfo::PostionInString) ui->comboPosition->currentIndex();
        m_charInfo->chCase =  ui->comboCases->currentIndex() == 0 ? CharInfo::TwoCases : CharInfo::FourCases;

        emit charInfoChanged();
    }
}
