#include "mainwindow.h"
//#include "enemyeditwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //EnemyEditWindow enemyEditWindow;
    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
