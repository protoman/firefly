#include "areaedittab.h"
#include "ui_areaedittab.h"

AreaEditTab::AreaEditTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditTab)
{
    ui->setupUi(this);
}

AreaEditTab::~AreaEditTab()
{
    delete ui;
}
