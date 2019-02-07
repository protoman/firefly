#include "slopeedittab.h"
#include "ui_slopeedittab.h"

SlopeEditTab::SlopeEditTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlopeEditTab)
{
    ui->setupUi(this);
}

SlopeEditTab::~SlopeEditTab()
{
    delete ui;
}
