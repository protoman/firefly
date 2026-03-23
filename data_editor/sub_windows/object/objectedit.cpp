#include "objectedit.h"
#include "ui_objectedit.h"

ObjectEdit::ObjectEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ObjectEdit)
{
    ui->setupUi(this);
}

ObjectEdit::~ObjectEdit()
{
    delete ui;
}
