#include "dialogeditform.h"
#include "ui_dialogeditform.h"

DialogEditForm::DialogEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogEditForm)
{
    ui->setupUi(this);
}

DialogEditForm::~DialogEditForm()
{
    delete ui;
}
