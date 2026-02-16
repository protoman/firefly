#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(ui->actionPlace_Enemy);
    toolGroup->addAction(ui->actionPlace_Object);
    toolGroup->setExclusive(true); // Only one action can be checked at a time
    ui->actionPlace_Enemy->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionEnemy_Editor_triggered()
{
    enemyEdit.start();
}


void MainWindow::on_actionPlace_Enemy_triggered()
{
    editMode = e_editMode_ENEMY;
}


void MainWindow::on_actionPlace_Object_triggered()
{
    editMode = e_editMode_OBJECT;
}

