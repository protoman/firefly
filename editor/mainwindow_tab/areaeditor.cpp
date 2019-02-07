#include "areaeditor.h"
#include "ui_areaeditor.h"

AreaEditor::AreaEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaEditor)
{
    ui->setupUi(this);
}

AreaEditor::~AreaEditor()
{
    delete ui;
}
