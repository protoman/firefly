#include "enemyeditwindow.h"
#include "./ui_enemyeditwindow.h"

EnemyEditWindow::EnemyEditWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

EnemyEditWindow::~EnemyEditWindow()
{
    delete ui;
}

void EnemyEditWindow::load_data()
{

}

void EnemyEditWindow::on_actionSave_triggered()
{

}

