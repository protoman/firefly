#include "questeditwindow.h"
#include "ui_questeditwindow.h"

QuestEditWindow::QuestEditWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QuestEditWindow)
{
    ui->setupUi(this);
}

QuestEditWindow::~QuestEditWindow()
{
    delete ui;
}
