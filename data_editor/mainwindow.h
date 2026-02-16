#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "enemy/enemyedit.h"

enum e_editMode {
    e_editMode_ENEMY,
    e_editMode_NPC,
    e_editMode_OBJECT
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionEnemy_Editor_triggered();

    void on_actionPlace_Enemy_triggered();

    void on_actionPlace_Object_triggered();

private:
    Ui::MainWindow *ui;
    EnemyEdit enemyEdit;
    e_editMode editMode = e_editMode_ENEMY;
};

#endif // MAINWINDOW_H
